from __future__ import unicode_literals
from django.db import models

from datetime import datetime

# Create your models here.
class Measurement(models.Model):
   timestamp = models.DateTimeField(default=datetime.now, blank = True)
   data = models.FloatField()
   typ = models.CharField(max_length=10)
   def __unicode__(self):
      return str(self.typ) + " Measurement @ " + str(self.timestamp)
   def __str__(self):
      return str(self.typ) + " Measurement @ " + str(self.timestamp)

class Temperature(models.Model):
   #check with lam regarding temp sigfigs
   num = models.IntegerField(default=1)
   recieved_at =  models.DateTimeField(default=datetime.now, blank = True)
   temp = models.DecimalField(max_digits=4, decimal_places=2)
   def __unicode__(self):
      return "Temperature Reading #" + str(self.num)
   def __str__(self):
      return "Temperature Reading #" + str(self.num)

class State(models.Model):
   esp = models.BooleanField(default=False)
   pump_1 = models.BooleanField(default=False)
   lights = models.BooleanField(default=False)
   pump_2 = models.BooleanField(default=False)
   
class Moisture(models.Model):
   num = models.IntegerField(default=1)
   received_at =  models.DateTimeField(default=datetime.now, blank = True)
   moisture = models.DecimalField(max_digits=4, decimal_places=2)
   def __unicode__(self):
      return "Moisture Reading #" + str(self.num)
   def __str__(self):
      return "Moisture Reading #" + str(self.num)
class SetPoint(models.Model):
   timestamp = models.DateTimeField(default=datetime.now, blank = True)
   lights = models.BooleanField(default=False)
   pump_1 = models.BooleanField(default=False)
   pump_2 = models.BooleanField(default=False)
   def __unicode__(self):
      return "Setpoint @ " + str(self.timestamp)
   def __str__(self):
      return "Setpoint @ " + str(self.timestamp)
