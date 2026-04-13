import ast
import ctypes
import json


PIPE_ACCESS_DUPLEX = 0x00000003
PIPE_TYPE_BYTE = 0x00000000
PIPE_READMODE_BYTE = 0x00000000
PIPE_WAIT = 0x00000000
PIPE_UNLIMITED_INSTANCES = 255
INVALID_HANDLE_VALUE = ctypes.c_void_p(-1).value
ERROR_PIPE_CONNECTED = 535

kernel32 = ctypes.WinDLL("kernel32", use_last_error=True)
kernel32.CreateNamedPipeW.restype = ctypes.c_void_p
kernel32.ConnectNamedPipe.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
kernel32.ConnectNamedPipe.restype = ctypes.c_int
kernel32.WriteFile.argtypes = [
    ctypes.c_void_p,
    ctypes.c_void_p,
    ctypes.c_uint32,
    ctypes.POINTER(ctypes.c_uint32),
    ctypes.c_void_p,
]
kernel32.WriteFile.restype = ctypes.c_int
kernel32.ReadFile.argtypes = [
    ctypes.c_void_p,
    ctypes.c_void_p,
    ctypes.c_uint32,
    ctypes.POINTER(ctypes.c_uint32),
    ctypes.c_void_p,
]
kernel32.ReadFile.restype = ctypes.c_int
kernel32.FlushFileBuffers.argtypes = [ctypes.c_void_p]
kernel32.FlushFileBuffers.restype = ctypes.c_int
kernel32.DisconnectNamedPipe.argtypes = [ctypes.c_void_p]
kernel32.DisconnectNamedPipe.restype = ctypes.c_int
kernel32.CloseHandle.argtypes = [ctypes.c_void_p]
kernel32.CloseHandle.restype = ctypes.c_int


class NamedPipeServer:
    def __init__(self, pipe_name: str) -> None:
        self.pipe_name = pipe_name
        self.handle = None
        self._read_buffer = ""

    @property
    def platform_path(self) -> str:
        return rf"\\.\pipe\{self.pipe_name}"

    def connect(self) -> None:
        self.handle = kernel32.CreateNamedPipeW(
            ctypes.c_wchar_p(self.platform_path),
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES,
            65536,
            65536,
            0,
            None,
        )
        if self.handle in (None, INVALID_HANDLE_VALUE):
            raise ctypes.WinError(ctypes.get_last_error())

        connected = kernel32.ConnectNamedPipe(self.handle, None)
        if not connected:
            error_code = ctypes.get_last_error()
            if error_code != ERROR_PIPE_CONNECTED:
                raise ctypes.WinError(error_code)

    def close(self) -> None:
        if self.handle not in (None, INVALID_HANDLE_VALUE):
            kernel32.FlushFileBuffers(self.handle)
            kernel32.DisconnectNamedPipe(self.handle)
            kernel32.CloseHandle(self.handle)
            self.handle = None

    def write_json(self, payload: dict) -> None:
        self.write_line(json.dumps(payload))

    def write_line(self, text: str) -> None:
        if self.handle is None:
            raise RuntimeError("Pipe is not connected")

        encoded = (text + "\n").encode("utf-8")
        buffer = ctypes.create_string_buffer(encoded)
        bytes_written = ctypes.c_uint32(0)
        success = kernel32.WriteFile(
            self.handle,
            buffer,
            len(encoded),
            ctypes.byref(bytes_written),
            None,
        )
        if not success:
            raise ctypes.WinError(ctypes.get_last_error())

    def read_json(self) -> dict:
        line = self.read_line()
        normalized_line = line.replace("\x00", "").strip()

        try:
            return json.loads(normalized_line)
        except json.JSONDecodeError as json_error:
            try:
                fallback_value = ast.literal_eval(normalized_line)
            except (SyntaxError, ValueError):
                raise RuntimeError(
                    f"Failed to parse control message. Raw line: {normalized_line!r}. JSON error: {json_error}"
                ) from json_error

            if not isinstance(fallback_value, dict):
                raise RuntimeError(
                    f"Control message parsed as a non-dict literal. Raw line: {normalized_line!r}"
                ) from json_error

            return fallback_value

    def read_line(self) -> str:
        while True:
            newline_index = self._read_buffer.find("\n")
            if newline_index >= 0:
                line = self._read_buffer[:newline_index]
                self._read_buffer = self._read_buffer[newline_index + 1 :]
                return line.strip()

            chunk = ctypes.create_string_buffer(4096)
            bytes_read = ctypes.c_uint32(0)
            success = kernel32.ReadFile(
                self.handle,
                chunk,
                len(chunk),
                ctypes.byref(bytes_read),
                None,
            )
            if not success or bytes_read.value == 0:
                raise RuntimeError("Pipe closed while waiting for data")

            self._read_buffer += chunk.raw[: bytes_read.value].decode("utf-8")
