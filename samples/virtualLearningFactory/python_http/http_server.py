from http.server import HTTPServer, BaseHTTPRequestHandler
from io import BytesIO
import subprocess
import urllib
import urllib.request
import signal
import requests

processes = {}

def startRoom(roomName):
    url1 = "http://srv.mvv.sztaki.hu/temp/vlft/virtualLearningFactory/rooms/" + roomName + "/apeVLFTSceneLoaderPlugin.json"
    url2 ="http://srv.mvv.sztaki.hu/temp/vlft/virtualLearningFactory/rooms/" + roomName + "/apeCore.json"
    print(url1)
    urllib.request.urlretrieve(url1, 'c:/ApertusVR/samples/virtualLearningFactory/apeVLFTSceneLoaderPlugin.json')
    urllib.request.urlretrieve(url2, 'c:/ApertusVR/samples/virtualLearningFactory/apeCore.json')
    
    processes[roomName] = subprocess.Popen([r'C:\ApertusVR-build\bin\release\apeSampleLauncher', 'C:/ApertusVR//samples//virtualLearningFactory'],creationflags=subprocess.CREATE_NEW_PROCESS_GROUP )
    print("started "+roomName)
def stopRoom(roomName):
    processes[roomName].send_signal(signal.CTRL_BREAK_EVENT)
    """processes[roomName].terminate()"""
    """del processes[roomName]"""
    URL = "http://195.111.2.95:8888/sessions/"+roomName
    r = requests.delete(URL)
    print(r)
    print("stopped "+roomName)

class SimpleHTTPRequestHandler(BaseHTTPRequestHandler):

    def do_GET(self):
        self.send_response(200)
        self.end_headers()

    def do_POST(self):
        content_length = int(self.headers['Content-Length'])
        body = self.rfile.read(content_length)
        self.send_response(200)
        self.end_headers()
        response = BytesIO()
        response.write(body)
        print(response.getvalue())
        msg = urllib.parse.unquote(response.getvalue().decode("utf-8"))
        print(msg)
        comm = msg[len("command="):]
        if comm[:len("startRoom")] == "startRoom":
                startRoom(comm[len("startRoom/:"):])
        elif comm[:len("stopRoom")] == "stopRoom":
                stopRoom(comm[len("stopRoom/:"):])
        self.wfile.write(bytes(msg, 'utf-8'))


httpd = HTTPServer(('0.0.0.0', 8888), SimpleHTTPRequestHandler)
httpd.serve_forever()