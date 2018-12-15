from __future__ import unicode_literals
from django.contrib import admin

# Register your models here.


from .models import Temperature, Moisture, SetPoint, Measurement, State 

admin.site.register(Temperature)
admin.site.register(Moisture)
admin.site.register(SetPoint)
admin.site.register(Measurement)
admin.site.register(State)
