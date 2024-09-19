#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# tests/tests.py

import pytest
import subprocess
import time
import requests
import os
import sys
import signal

@pytest.fixture(scope='module', autouse=True)
def server():

    server_cmd = ['src/web-server.exe'] if sys.platform == 'win32' else ['./src/web-server']
    
    # Start the server
    if sys.platform == 'win32':
        server_process = subprocess.Popen(server_cmd, cwd=os.path.abspath('..'), creationflags=subprocess.CREATE_NEW_PROCESS_GROUP)
    else:
        server_process = subprocess.Popen(server_cmd, cwd=os.path.abspath('..'))

    time.sleep(1)

    yield

    # Terminate the server gracefully
    if sys.platform == 'win32':
        server_process.send_signal(signal.CTRL_BREAK_EVENT)
    else:
        server_process.terminate()

    server_process.wait()


def test_get_index_html():
    response = requests.get('http://0.0.0.0:80/index.html')
    assert response.status_code == 200
    assert "Welcome to My Simple Web Server!" in response.text


def test_get_about_html():
    response = requests.get('http://0.0.0.0:80/about.html')
    assert response.status_code == 200
    assert 'About This Server' in response.text


def test_404_error():
    response = requests.get('http://0.0.0.0:80/nonexistent.html')
    assert response.status_code == 404


def test_post_echo():
    post_data = {'key': 'value'}
    response = requests.post('http://0.0.0.0:80/echo', data=post_data)    
    assert response.status_code == 200

def test_unsupported_method_put():
    response = requests.put('http://0.0.0.0:80/index.html')
    assert response.status_code == 405


def test_unsupported_method_delete():
    response = requests.delete('http://0.0.0.0:80/index.html')
    assert response.status_code == 405


if __name__ == '__main__':
    pytest.main(['-v', 'tests.py'])