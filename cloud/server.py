#!/usr/bin/python3
from datetime import datetime
from flask import Flask, request, Response, render_template
import json
import pandas as pd
from pandas import Timestamp
import plotly.express as px
import plotly.utils as pxl 
import plotly
import plotly.graph_objects as go
from plotly.subplots import make_subplots

app = Flask(__name__)
filepath1 = r"*****" #FILEPATH

@app.route('/graph')
def plotly():
    minutes = int(request.args.get('minutes'))

    f = open(filepath1, "r")
    results = []

    for line in f:
        # print(line)
        line = line.split('&')
        try:
            results.append({
                "time": pd.Timestamp(line[0].split('=')[1]),
                "id": line[1].split('=')[1],
                "T1": kelvin2celc(line[2].split('=')[1]),
                "T2": kelvin2celc(line[3].split('=')[1]),
                "DTO": kelvin2celc(line[4].split('=')[1]),
                "DHO": int(line[5].split('=')[1]),
                "DTI": kelvin2celc(line[6].split('=')[1]),
                "DHI": int(line[7].split('=')[1]),
                "DUST": int(line[8].split('=')[1])/100
            })
        except Exception:
            pass

    indexes = map(lambda x: x['time'], results)
    data = pd.DataFrame(data=results, index=indexes)
    data['time'] = data['time'].apply(lambda dt: datetime(dt.year, dt.month, dt.day, dt.hour, minutes*(dt.minute // minutes)))
    prepared = data.groupby(by='time').mean()
    prepared['time'] = pd.unique(data['time'])
    fig = make_subplots(specs=[[{"secondary_y": True}]])
    fig.update_xaxes(rangeslider_visible=True)
    
    fig.add_trace(go.Scatter(x=prepared['time'], y= prepared['DTO'],name = "DTO"), secondary_y=False,)
    
    fig.add_trace(go.Scatter(x=prepared['time'], y=prepared['DHO'], name="DHO"), secondary_y=False,)
    
    fig.add_trace(go.Scatter(x=prepared['time'], y=prepared['DTI'], name="DTI"), secondary_y=False,)    

    fig.add_trace(go.Scatter(x=prepared['time'], y=prepared['DHI'], name="DHI"), secondary_y=False,)
        
    fig.add_trace(go.Scatter(x=prepared['time'], y=prepared['T1'], name="TI"), secondary_y=False,)

    fig.add_trace(go.Scatter(x=prepared['time'], y=prepared['T2'], name="TO"), secondary_y=False,)
        
    fig.add_trace(go.Scatter(x=prepared['time'], y=prepared['DUST'], name="DUST", visible='legendonly'), secondary_y=False,)
        
    graphJSON = json.dumps(fig, cls=pxl.PlotlyJSONEncoder)
    return render_template('plotly.html', graphJSON=graphJSON)

@app.route('/arduino', methods=['POST'])
def index():

    print(str(request.form))

    if request.get_data()[3:7].decode('Ascii') == "****":

        now = datetime.now()

        buff = request.get_data().decode('Ascii')
        print(buff)
        if len(buff) != 70:
            return "xD"
        f = open(filepath1, "a")
        f.write(str("time=") + now.strftime("%Y-%m-%d %H:%M:%S") + str("&"))
        f.write(buff)
        f.write(str("\n"))
        f.close()
        return "xD"
    else:
        return "xD"

@app.route('/average', methods=['GET'])
def average_data():
    minutes = int(request.args.get('minutes'))

    f = open(filepath1, "r")
    results = []

    for line in f:
        # print(line)
        line = line.split('&')
        try:
            results.append({
                "time": pd.Timestamp(line[0].split('=')[1]),
                "id": line[1].split('=')[1],
                "T1": kelvin2celc(line[2].split('=')[1]),
                "T2": kelvin2celc(line[3].split('=')[1]),
                "DTO": kelvin2celc(line[4].split('=')[1]),
                "DHO": int(line[5].split('=')[1]),
                "DTI": kelvin2celc(line[6].split('=')[1]),
                "DHI": int(line[7].split('=')[1]),
                "DUST": int(line[8].split('=')[1])/100
            })
        except Exception:
            pass

    indexes = map(lambda x: x['time'], results)
    #print(list(indexes))
    data = pd.DataFrame(data=results, index=indexes)
    data['time'] = data['time'].apply(lambda dt: datetime(dt.year, dt.month, dt.day, dt.hour, minutes*(dt.minute // minutes)))
    
    return Response(data.groupby('time').mean().to_json(date_format='iso'), mimetype='application/json')

@app.route('/results', methods=['GET'])
def getDataREST():
    f = open(filepath1, "r")
    results = []

    for line in f:
        line = line.split('&')
        try:
            results.append({
                "time": line[0].split('=')[1],
                "id": line[1].split('=')[1],
                "T1": kelvin2celc(line[2].split('=')[1]),
                "T2": kelvin2celc(line[3].split('=')[1]),
                "DTO": kelvin2celc(line[4].split('=')[1]),
                "DHO": int(line[5].split('=')[1]),
                "DTI": kelvin2celc(line[6].split('=')[1]),
                "DHI": int(line[7].split('=')[1]),
                "DUST": int(line[8].split('=')[1])/100
            })
        except Exception:
            pass

    return Response(json.dumps(results), mimetype='application/json')

def kelvin2celc(temp):
    return (int(temp) - 27315) / 100


if __name__ == '__main__':
    app.run(host="0.0.0.0", port=**) #PORT
