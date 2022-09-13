import os
import datetime
import requests
from dotenv import load_dotenv

load_dotenv()

try:

    url = os.getenv('API_URL') + "/upload/KH-GEN-001"
    data_folder = 'data'

    while True:
        folder = os.path.join(data_folder, datetime.datetime.now().strftime('%Y-%m-%d'))

        if not os.path.exists(folder):
            break

        files = os.listdir(folder)
        currminute = datetime.datetime.now().strftime('%Y-%m-%d %H:%M')
        for filename in files:
            if filename != currminute + '.csv':
                with open(folder + '/' + filename, 'r') as f:
                    data = f.read()
                    print(data)
                    response = requests.post(url, data={"data": data})
                    f.close()
                    print(response.content)

                os.unlink(folder + '/' + filename)

        #os.rmdir(folder) @todo remove previous days' directories.
        break

except Exception as e:
    print("Error: \n")
    print(e)
