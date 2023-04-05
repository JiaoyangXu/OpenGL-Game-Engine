root = gr.node('root')


red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
blue = gr.material({0.0, 0.4, 0.8}, {0.1, 0.1, 0.1}, 10)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)

yellow = gr.material({1.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
black = gr.material({0.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)



face_joint = gr.joint('face_joint', {-50, 0, 50}, {0, 0, 0} )
left_eye_joint = gr.joint('left_eye_joint', {-50, 0, 50}, {0, 0, 0} )
right_eye_joint = gr.joint('right_eye_joint', {-50, 0, 50}, {0, 0, 0} )

left_eye_joint:translate(-0.3, 0.0, 0.0)
right_eye_joint:translate(0.3, 0.0, 0.0)

left_eye = gr.mesh('sphere', 'left_eye')
right_eye = gr.mesh('sphere', 'right_eye')
left_eyeball = gr.mesh('sphere', 'left_eyeball')
right_eyeball = gr.mesh('sphere', 'right_eyeball')
mouse = gr.mesh("cube", "mouth")

left_eye:scale(0.3, 0.4, 0.1)
right_eye:scale(0.3, 0.4, 0.1)
mouse:scale(1.0, 0.04,0.1)

mouse:translate(0, -0.9, 0.0)

left_eye:set_material(white)
right_eye:set_material(white)
mouse:set_material(black)

left_eyeball:scale(0.15, 0.2, 0.1)
right_eyeball:scale(0.15, 0.2, 0.1)


left_eyeball:translate(0.1, 0.0, 0.0)
right_eyeball:translate(-0.1, 0.0, 0.0)
left_eyeball:set_material(black)
right_eyeball:set_material(black)

left_eye_joint:add_child(left_eye)
left_eye_joint:add_child(left_eyeball)
right_eye_joint:add_child(right_eye)
right_eye_joint:add_child(right_eyeball)


face_joint:add_child(left_eye_joint)
face_joint:add_child(right_eye_joint)
face_joint:add_child(mouse)


face_joint:translate(2.0, 1.4, 1.5)

root:add_child(face_joint)


--Head
head = gr.mesh('sphere', 'head')
head:scale(1.5,1.5,1.5)
head:set_material(blue)
head:translate(2.0, 1.0, 0.0)


root:add_child(head)
root:translate(-3.0, 0.0, -10.0)

--torso
torso = gr.mesh('sphere', 'torso')
torso:scale(1.2, 1.5, 1.5 )
torso:translate(2.0, -1, 0)
torso:set_material(blue)

root:add_child(torso)

--left leg
left_leg = gr.mesh("sphere", 'left_leg')
left_leg:scale(0.4, 0.5,1)
left_leg:translate(1.5, -2.7, 0)
left_leg:set_material(blue)

left_leg_bottom = gr.mesh("sphere", 'left_leg_bottom')
left_leg_bottom:scale(0.4, 0.5,1)
left_leg_bottom:translate(1.5, -3.3, 0)
left_leg_bottom:set_material(blue)

--right leg
right_leg = gr.mesh("sphere", 'right_leg')
right_leg:scale(0.4, 0.5,1)
right_leg:translate(2.5, -2.7, 0)
right_leg:set_material(blue)

right_leg_bottom = gr.mesh("sphere", 'right_leg_bottom')
right_leg_bottom:scale(0.4, 0.5,1)
right_leg_bottom:translate(2.5, -3.3, 0)
right_leg_bottom:set_material(blue)



root:add_child(left_leg)
root:add_child(left_leg_bottom)
root:add_child(right_leg)
root:add_child(right_leg_bottom)


-- right arm
right_arm = gr.mesh("sphere", 'right_arm')
right_arm:scale(1, 0.3,1)
right_arm:translate(3.5, -1, 0)
right_arm:rotate('z', 10)
right_arm:set_material(blue)

right_arm_bottom = gr.mesh("sphere", 'right_arm_bottom')
right_arm_bottom:scale(1, 0.3,1)
right_arm_bottom:translate(4.3, -1, 0)
right_arm_bottom:rotate('z', 10)
right_arm_bottom:set_material(blue)
root:add_child(right_arm)
root:add_child(right_arm_bottom)


--left arm
left_arm = gr.mesh("sphere", 'left_arm')
left_arm:scale(1, 0.3,1)
left_arm:translate(0.3,-0.5,0)
left_arm:set_material(blue)

--left bottom arm
left_bottom_arm = gr.mesh("sphere", 'left_bottom_arm')
left_bottom_arm:scale(1, 0.3,1)
left_bottom_arm:translate(0.9,-0.5,0)
left_bottom_arm:set_material(blue)

root:add_child(left_arm)
root:add_child(left_bottom_arm)
return root