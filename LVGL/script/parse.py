#!/usr/bin/env python
import requests
from bs4 import BeautifulSoup

req = requests.get("https://docs.lvgl.io/master/API/core/lv_obj.html")
bsObj = BeautifulSoup(req.text, "html.parser")

items = bsObj.find_all("dt")
for item in items:
    print(item.get("id"))
