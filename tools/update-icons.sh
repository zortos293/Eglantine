#!/bin/sh -ex

# SPDX-FileCopyrightText: Copyright 2026 Lichii Project
# SPDX-License-Identifier: GPL-3.0-or-later

# Updates main icons for lichii

which magick || exit
which optipng || exit

LICHII_SVG="dist/lichii.svg"

[ -f "$LICHII_SVG" ] || { echo "Error: missing $LICHII_SVG" >&2; exit; }

# Desktop / Windows / Qt icons

magick -density 256x256 -background transparent "$LICHII_SVG" -define icon:auto-resize -colors 256 dist/lichii.ico || exit
magick -density 256x256 -background black "$LICHII_SVG" -resize 256x256 dist/lichii.bmp || exit
magick -density 256x256 -background transparent "$LICHII_SVG" -resize 256x256 dist/lichii.png || exit

# macOS

TMP_PNG="dist/lichii-tmp.png"

magick -size 1024x1024 -background none "$LICHII_SVG" "$TMP_PNG" || exit

png2icns dist/lichii.icns "$TMP_PNG" || echo 'non fatal'

rm "$TMP_PNG"
