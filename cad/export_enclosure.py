#!/usr/bin/env python3
"""Export the enclosure STEP and STL files from the FreeCAD model.

Run with:
    freecadcmd cad/export_enclosure.py
"""

import os

import FreeCAD as App
import Mesh
import Part

HERE = os.path.dirname(os.path.abspath(__file__))
DOC_PATH = os.path.join(HERE, "Macropad_Enclosure.FCStd")
STEP_PATH = os.path.join(HERE, "Macropad_Enclosure.step")
BOTTOM_STL = os.path.join(HERE, "enclosure_bottom.stl")
TOP_STL = os.path.join(HERE, "enclosure_top.stl")

doc = App.openDocument(DOC_PATH)
bottom = doc.getObject("Bottom_Case")
top = doc.getObject("Top_Cover")
if bottom is None or top is None:
    raise RuntimeError("Bottom_Case and Top_Cover not found in the model")

Part.export([bottom, top], STEP_PATH)
Mesh.export([bottom], BOTTOM_STL)
Mesh.export([top], TOP_STL)
print("exported STEP and STL files")
