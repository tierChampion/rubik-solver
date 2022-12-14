# Rubik Solver

![rubik cube](https://github.com/tierChampion/rubik-solver/blob/master/res/Images/rubik_scramble.png)

![mirror cube](https://github.com/tierChampion/rubik-solver/blob/master/res/Images/mirror_scramble.png)

![gem cube](https://github.com/tierChampion/rubik-solver/blob/master/res/Images/gem_scramble.png)

## Rendering

The rendering is done with OpenGL by rendering 26 fully-colored cubies. In the case of the mirror cube, the scaling in each direction is changed depending on the cubie.

## Solving

The solving algorithm is a mix between the two most popular optimal solving algorithms: Thistlethwaite's and Kociemba's. Both of them are based on the idea of restricting moves by reaching positions that don't need these specific moves. In the implementation, this is done with a bidirectional breadth-first search of all the relevant positions for each of the sub-problems.

The original Thistlethwaite's algorithm has 4 different phases. The first phase only looks at the orientation of the edges and considers all moves, while the second phase looks at the orientation of the corners as well as the position of the edges that belong in the middle row and removing the single turn moves on the front and back faces. The third and most complicated phase, looks at the general position of the edges and corners, making sure they are half turns away from their solved positions. Where as the third phase only does single turns with the up and down faces, the last phase only allows double turns while looking at the whole cube.

Kociemba's algorithm however only has 2 phases. The first one is basically a mix between the two first phases of the previous algorithm and considers the orientation of all the pieces. The second and last phase only allows single turns on the up and down phases and look at the permutations of the corners, edges and edges in the middle row.

The problem with the Kociemba is that the number of possible positions is way too much in the second phase so tables are needed. However, by doing the third phase of the Thistlethwaite's algorithm before the second phase of the Kociemba's, no tables are needed anymore.

## Slicing

It is also possible to use a mesh chosen by the user for the general shape of the cube. This process of slicing will essentialy take the given mesh and slice it into three sections on each axis making 26 distinct parts that will then be used for each of the cubies. The problem basically boils down to the triangulation of the polygon created by the intersection between the slicing plane and the mesh. However, some shapes are not supported. 

### Conditions for slicing

The only condition to respect is that all intersections between a slicing plane and the mesh must be a polygon with no holes. To check if a custom mesh would work, the blender project [rubik_checker.blend](https://github.com/tierChampion/rubik-solver/blob/master/res/MeshChecker/) can be used.

## Parameters

A few command line parameters are available with the executable that can be enumerated with the '/?' parameter. Primarely, a Mirror cube can be used instead of a Rubik's cube. A mirror cube only has a single color but the shape of each cubie is different. There is also the slicing feature talked about above.
