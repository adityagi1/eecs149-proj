from django import forms

class IntegerForm(forms.Form):
    water_heater = forms.IntegerField(label='Heater Temperature Setpoint', max_value=100, min_value=-100)
