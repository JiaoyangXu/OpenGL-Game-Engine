-- Simple Scene:
-- An extremely simple scene that will render right out of the box
-- with the provided skeleton code.  It doesn't rely on hierarchical
-- transformations.

-- Create the top level root node named 'root'.
rootNode = gr.node('root')

cubeMesh2 = gr.mesh('11088_balloon_v3', 'my_cube')
cubeMesh2:scale(1.0, 1.0, 1.0)
cubeMesh2:rotate('y', 45.0)
cubeMesh2:translate(-0.8, -1.0, -2.0)
cubeMesh2:set_material(gr.material({0.8, 0.2, 0.2}, {0.8, 0.8, 0.8}, 10.0))


-- cubeMesh = gr.mesh('Cube', 'my_cube')
-- cubeMesh2:scale(0.05, 0.05, 0.05)
-- cubeMesh2:rotate('y', 45.0)
-- cubeMesh2:translate(0.0, -0.5, -3.0)
-- cubeMesh2:set_material(gr.material({0.8, 0.2, 0.2}, {0.8, 0.8, 0.8}, 10.0))


-- Add the sphereMesh GeometryNode to the child list of rootnode.
rootNode:add_child(cubeMesh2)
-- rootNode:add_child(cubeMesh)


-- Return the root with all of it's childern.  The SceneNode A3::m_rootNode will be set
-- equal to the return value from this Lua script.
return rootNode
