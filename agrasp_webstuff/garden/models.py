# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models
from datetime import datetime

# Create your models here.


class Garden(models.Model):
    temperature = models.DecimalField(max_digits=4, decimal_places=2)
    moisture = models.DecimalField(max_digits=4, decimal_places=2)
    created_at =  models.DateTimeField(default=datetime.now, blank = True)

