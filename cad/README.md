# cad

Parametric FreeCAD model and STL meshes for the 3x3 macropad case.

The case was drawn around the `95.25 x 95.885 mm` KiCad board outline. The PCB tray is `95.75 x 96.385 mm`, which leaves 0.25 mm clearance per side. The walls are 3 mm.

## dimensions

- outer size: `101.75 x 102.385 mm`
- typing angle: `6.5 degrees`
- front rim height: `10.10 mm`
- rear rim height: `21.77 mm`
- PCB shelf: 1.5 mm wide, at `Z = 8.50 + y*tan(6.5 deg)`
- diode cavity: 6 mm deep under the PCB
- rear USB opening: `14 x 6.7 mm`, aligned with the Pico
- switch cutouts: nine `14 x 14 mm` squares
- encoder cutout: `12.5 mm` diameter
- OLED window: `28 x 15 mm`

The top cover has four cantilever snap-fit hooks in the side walls. The hooks have a 0.8 x 0.8 mm lead-in chamfer and a 0.30 mm catch. The bottom case has matching slots and undercut pockets.

## files

- `Macropad_Enclosure.FCStd`
- `Macropad_Enclosure.step`
- `enclosure_bottom.stl`
- `enclosure_top.stl`
- `export_enclosure.py`

The FreeCAD project and STEP export are included. The model contains `Bottom_Case` and `Top_Cover` bodies, outer-edge fillets, and a `Parameters` spreadsheet. Run `freecadcmd cad/export_enclosure.py` with FreeCAD installed to re-export the STEP and STL files from the model.

## printing

- PLA or PETG
- 0.2 mm layer height
- 3 walls
- 20% infill
- no supports

Print the bottom case flat on its base. Print the top cover upside down so the top face is on the bed and the hooks point up.

## assembly

1. Solder the switches, diodes, encoder and OLED header to the PCB.
2. Drop the PCB into the bottom case. The board edges should sit on the 1.5 mm shelf and the Pico USB connector should line up with the rear opening.
3. Press the top cover down until the four hooks click into the side pockets. The switch bodies should pass through the plate cutouts.
