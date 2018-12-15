from django.shortcuts import render
from django.http import HttpResponse
from django.http import HttpResponseRedirect
from .models import Temperature, Moisture
from datetime import datetime, timedelta
# Create your views here.
import django
from .forms import IntegerForm
from matplotlib.backends.backend_agg import FigureCanvasAgg as FigureCanvas
from matplotlib.figure import Figure
import numpy as np
import io
from .models import Measurement, State
from itertools import chain
import os
def heater(request):
   heat = Measurement.objects.filter(typ="h_temp").latest("timestamp")
   if request.method == 'POST':
      form = IntegerForm(request.POST)
      if form.is_valid():
         string = "mosquitto_pub -h agrasp.wifizone.org -p 80 -t \"testing2\" -i yes -m \"" + str(form.cleaned_data['water_heater']) + "\""      
         os.system(string)
         return render(request, 'garden/heater.html', {'form': IntegerForm(), 'heat':heat})
   else:
      form = IntegerForm()
   return render(request, 'garden/heater.html', {'form': form, 'heat':heat})
def index(request):
   return render(request, 'garden/index.html')
def gard(request):
   st = State.objects.all()[0]
   tempL = Measurement.objects.filter(typ="temp").latest("timestamp")
   s1L = Measurement.objects.filter(typ="soil_1").latest("timestamp")
   s2L = Measurement.objects.filter(typ="soil_2").latest("timestamp")
   water= Measurement.objects.filter(typ="water").latest("timestamp")
   context = {'tempL' : tempL, 'soil1': s1L, 'soil2': s2L, 'water':water, 'state': st}
   return render(request, 'garden/gd.html', context)
def simple(request):
   fig = Figure()
   meas = Measurement.objects.filter(typ="temp")
   h = meas.latest("timestamp").timestamp.hour
   ax = fig.add_subplot(111)
   y = np.array([m.data for m in meas if m.timestamp.hour == h])
   x = np.array([m.timestamp - timedelta(hours=8) for m in meas if m.timestamp.hour ==h])
   fig.suptitle('Temperature History (past hour)')
   ax.set_xlabel('Time')
   ax.set_ylabel('Temperature (Celsius)')
   ax.plot(x, y)
   canvas = FigureCanvas(fig)
   buf = io.BytesIO()
   canvas.print_png(buf)
   response=HttpResponse(buf.getvalue(), content_type='image/png')
   return response
def h_simple(request):
   fig = Figure()
   meas = Measurement.objects.filter(typ="h_temp")
   h = meas.latest("timestamp").timestamp.hour
   ax = fig.add_subplot(111)
   y = np.array([m.data for m in meas if m.timestamp.hour == h])
   x = np.array([m.timestamp- timedelta(hours=8) for m in meas if m.timestamp.hour ==h])
   fig.suptitle('Temperature History (past hour)')
   ax.set_xlabel('Time')
   ax.set_ylabel('Temperature (Celsius)')
   ax.plot(x, y)
   canvas = FigureCanvas(fig)
   buf = io.BytesIO()
   canvas.print_png(buf)
   response=HttpResponse(buf.getvalue(), content_type='image/png')
   return response
def soil(request):
   fig = Figure()
   meas = Measurement.objects.filter(typ="soil_1")
   h = meas.latest("timestamp").timestamp.hour
   ax = fig.add_subplot(111)
   y = np.array([m.data for m in meas if m.timestamp.hour == h])
   x = np.array([m.timestamp- timedelta(hours=8) for m in meas if m.timestamp.hour == h])
   fig.suptitle('Plant 1 Moisture History (past hour)')
   ax.set_xlabel('Time')
   ax.set_ylabel('Moisture %')
   ax.plot(x, y)
   canvas = FigureCanvas(fig)
   buf = io.BytesIO()
   canvas.print_png(buf)
   response=HttpResponse(buf.getvalue(), content_type='image/png')
   return response
def soil2(request):
   fig = Figure()
   meas = Measurement.objects.filter(typ="soil_2")
   h = meas.latest("timestamp").timestamp.hour
   ax = fig.add_subplot(111)
   y = np.array([m.data for m in meas if m.timestamp.hour == h])
   x = np.array([m.timestamp- timedelta(hours=8) for m in meas if m.timestamp.hour == h])
   fig.suptitle('Plant 2 Moisture History (past hour)')
   ax.set_xlabel('Time')
   ax.set_ylabel('Moisture %')
   ax.plot(x, y)
   canvas = FigureCanvas(fig)
   buf = io.BytesIO()
   canvas.print_png(buf)
   response=HttpResponse(buf.getvalue(), content_type='image/png')
   return response
def switch_p1(request):
   s = State.objects.all()[0]
   bit0, bit1, bit2, bit3 = "1" if s.esp else "0", "0" if s.pump_1 else "1", "1" if s.lights else "0", "1" if s.pump_2 else "0"
   s.pump_1 = bit1
   info = bit0 + bit1 + bit2 + bit3
   string = "mosquitto_pub -h agrasp.wifizone.org -p 80 -t \"testing\" -i yes -m \""+str(info)+"\""
   os.system(string)
   s.save()
   return gard(request)
def switch_p2(request):
   s = State.objects.all()[0]
   bit0, bit1, bit2, bit3 = "1" if s.esp else "0", "1" if s.pump_1 else "0", "1" if s.lights else "0", "0" if s.pump_2 else "1"
   s.pump_2 = bit3
   info = bit0 + bit1 + bit2 + bit3
   string = "mosquitto_pub -h agrasp.wifizone.org -p 80 -t \"testing\" -i yes -m \""+str(info)+"\""
   os.system(string)
   s.save()
   return gard(request)
def lights_on(request):
   s = State.objects.all()[0]
   bit0, bit1, bit2, bit3 = "1" if s.esp else "0", "1" if s.pump_1 else "0", "0" if s.lights else "1", "1" if s.pump_2 else "0"
   s.lights = bit2
   info = bit0 + bit1 + bit2 + bit3
   string = "mosquitto_pub -h agrasp.wifizone.org -p 80 -t \"testing\" -i yes -m \""+str(info)+"\""
   os.system(string)
   s.save()
   return gard(request)
def submit(request):
    info=request.POST['info']
    string = "mosquitto_pub -h agrasp.wifizone.org -p 80 -t \"testing\" -i yes -m \""+str(info)+"\""
    os.system(string)
    print("FUCKCKCKCKCCKCKKCCKKCCKKCKCKCKKKKKCKKCCKCKCKC")
    return gard(request)

