import os
import sys
import traceback


def bootstrap_script_directory() -> None:
    script_directory = os.path.dirname(os.path.abspath(__file__))
    if script_directory not in sys.path:
        sys.path.insert(0, script_directory)


def run() -> int:
    bootstrap_script_directory()

    from geometry_nodes_bridge_worker.worker import main as worker_main

    return worker_main(sys.argv[1:])


if __name__ == "__main__":
    try:
        sys.exit(run())
    except Exception:  # pylint: disable=broad-except
        traceback.print_exc()
        sys.exit(1)
