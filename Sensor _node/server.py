import socket
import datetime
import os

r = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
r.bind(('0.0.0.0', 8091))
r.listen(0)
data_folder = 'data'

while True:
    client, addr = r.accept()
    
    while True:
        content = client.recv(100)
        
        if len(content) == 0:
            break

        else:
            folder = os.path.join(data_folder, datetime.datetime.now().strftime('%Y-%m-%d'))
            if not os.path.exists(folder):
                os.makedirs(folder)
            date = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
            filename = datetime.datetime.now().strftime('%Y-%m-%d %H:%M')

            with open(os.path.join(folder, filename + '.csv'), 'a') as f:
                data = date + "," + content.decode('ascii') + "\r\n"
                f.write(data)
                f.close()

            print(date + "," + content.decode('ascii'))

client.close()
