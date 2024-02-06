# OSM Region Download

This is a small tool that allows users to download and merge OpenStreetMap (OSM) tiles based on specified geographic coordinates. While OSM allows you to do this, the user is limited in the size of the grid they can specify. For grids too big, this will request smaller individual grids, then glue them all together - still allowing the user to fine tune the coordinates they want.

While this should work for larger bounding boxes, it is just meant to be used on relatively small areas, as their API is not meant for bulk downloads. To anyone looking to download large areas, check out [here](https://wiki.openstreetmap.org/wiki/Planet.osm#Downloading) for downloading **Earth**, or [here](https://wiki.openstreetmap.org/wiki/Downloading_data#Huge_amounts_of_data) for specific regions.

## Features

- Download OSM tiles by specifying geographic boundaries (north, east, south, west).
- Merge downloaded tiles into a single OSM file.
- Customize tile dimensions.

## Prerequisites

1. **libcurl:**

    ```bash
    sudo apt-get install libcurl4-openssl-dev
    ```

2. **osmctools:**

    ```bash
    sudo apt-get install osmctools
    ```

## Build

Compile with the `-lcurl` flag. i.e.:

```bash
g++ region.cpp -o osm-region-download -lcurl
```

## Usage

On OSM, just plug in the coordinates you need, then input them here. Coordinates and tile size can be input/modified via source or command line.
By default, the width of the bounding box is 0.005, with a height of 0.003. This should be fine for most places, but if your box encompasses a very dense region, you may need to decrease it.</br> 
Ex:
 
```bash
./osm-region-download --width <tileWidth> --height <tileHeight> --north <north> --east <east> --south <south> --west <west>
```
