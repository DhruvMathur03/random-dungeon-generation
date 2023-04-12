# Random Dungeon Generation

The fundamental idea behind random generation revolved around constructing chambers and dynamically connecting them through passages. The original intention was to position five chambers randomly throughout the floor; however, this approach presented several challenges:
- The floor often appeared lopsided, as the majority of chambers were concentrated on one side, resulting in an unbalanced layout.
- The close proximity of the chambers further complicated their interconnection, due to the tight spacing.
To overcome these obstacles, I devised a new strategy: dividing the floor into five approximately equal sections, with each section containing a randomly generated chamber. To create each chamber, I generated 6 to 10 random points within the respective sections, subsequently connecting them to form the chamber's basic outline. We then filled these shapes with floor tiles and enclosed them with wall tiles, successfully generating our random chambers.
The final task was to interconnect the chambers using passages. We developed a variety of passage templates as seeds, each with an equal probability of being selected. This approach enabled us to establish seamless connections between the chambers, resulting in a more visually appealing and functional floor design.
