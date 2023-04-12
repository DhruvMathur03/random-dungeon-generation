# Random Dungeon Generation

The fundamental idea behind random generation revolved around constructing chambers and dynamically connecting them through passages. The original intention was to position five chambers randomly throughout the floor; however, this approach presented several challenges:
- The floor often appeared lopsided, as the majority of chambers were concentrated on one side, resulting in an unbalanced layout.
- The close proximity of the chambers further complicated their interconnection, due to the tight spacing.

To overcome these obstacles, I devised a new strategy: dividing the floor into five approximately equal sections, with each section containing a randomly generated chamber. To create each chamber, I generated 6 to 10 random points within the respective sections, subsequently connecting them to form the chamber's basic outline. We then filled these shapes with floor tiles and enclosed them with wall tiles, successfully generating our random chambers.
The final task was to interconnect the chambers using passages. We developed a variety of passage templates as seeds, each with an equal probability of being selected. This approach enabled us to establish seamless connections between the chambers, resulting in a more visually appealing and functional floor design.


# Dungeon Detection Algorithm

I implemented a dungeon detection algorithm as well, applying the flood-fill algorithm.

This program provides a robust chamber corner detection algorithm that detects the corners of chambers in a given 2D floor map represented as a grid of characters. The algorithm uses depth-first search (DFS) to traverse the floor map and identify chamber corners.

## Algorithm Description

The algorithm consists of two main functions: 'dfs()' and 'detect_chamber_corner_tiles()'.

### dfs()

This function performs a depth-first search on the floor map starting from a specified row and column. It takes the following parameters:

- floor_map: A 2D vector of strings representing the floor map.
- row: The row number of the starting cell.
- col: The column number of the starting cell.
- corner_tiles: A vector to store the corner tiles found during the DFS traversal.
- visited: A 2D vector of booleans to keep track of visited cells.

The function checks if the current cell is within the floor map boundaries and if it has not been visited. If these conditions are met, the function proceeds to calculate the number of adjacent walls to the current cell. If the number of adjacent walls is equal to or greater than 2, the current cell is considered a corner and added to the corner_tiles vector. The function then proceeds with the DFS traversal in all four cardinal directions.

### detect_chamber_corner_tiles()

This function detects and groups the corner tiles of chambers in the floor map. It takes the following parameter:

- floor_map: A 2D vector of strings representing the floor map.

The function initializes an empty 2D vector chambers_corner_tiles to store the corner tiles of each chamber and a 2D vector visited to keep track of visited cells. It then iterates through each cell in the floor map. If the cell is a floor ('.') and has not been visited, the dfs() function is called with the current cell as the starting point. The corner tiles found during the DFS traversal are added as a new chamber in the chambers_corner_tiles vector.
The function returns chambers_corner_tiles, a 2D vector containing the corner tiles of each chamber.

## Usage

To use this algorithm, simply provide a 2D vector of strings representing the floor map, where '.' denotes floor cells, '|' and '-' denote wall cells, and other characters represent obstacles or empty spaces. Call the detect_chamber_corner_tiles() function with the floor map as a parameter. The function will return a 2D vector containing the corner tiles of each chamber in the floor map.
