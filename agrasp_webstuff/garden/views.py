# -*- coding: utf-8 -*-
from __future__ import unicode_literals
from django.http import HttpResponse
from django.shortcuts import render

from .models import Garden

# Create your views here.

def index(request):

    gardens = Garden.objects.all()[:10]

    context = {
        'gardens' : gardens
    }
    return render(request, 'garden/index.html', context)
