#!/usr/bin/env bash
cd "$( dirname "${BASH_SOURCE[0]}" )"/../..

python3 -m pip install pytest
python3 -m pip install --find-links=dist qpalm
pytest -rP
