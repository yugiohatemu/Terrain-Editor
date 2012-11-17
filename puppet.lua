--
-- CS488 -- Introduction to Computer Graphics
-- 
-- puppet.lua
--
-- A very simple scene creating a trivial puppet.  The TAs will be
-- using this scene as part of marking your assignment.  You'll
-- probably want to make sure you get reasonable results with it!

rootnode = gr.node('root')

red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)


body = gr.sphere('body')
body:set_material(white)
body:scale(1,2.5,1);
rootnode:add_child(body)

shoulder = gr.sphere('shoulder')
shoulder:set_material(white)
shoulder:scale(2,0.5,1)
shoulder:translate(0,2,0)
--rootnode:add_child(shoulder)
body:add_child(shoulder)

neck_joint = gr.joint('neckjoint',{-30,0,30},{-30,0,30})
shoulder:add_child(neck_joint)

neck = gr.sphere('neck')
neck:set_material(white)
neck:translate(0,0.8,0)
neck:scale(0.5,0.5,0.5)

neck_joint:add_child(neck)

head_joint = gr.joint('headjoint',{-30,0,30},{-30,0,30})
neck:add_child(head_joint)

head = gr.sphere('head')
head:set_material(white)
head:translate(0,1,0)
--head:scale(0.5,1.5,0.5)
--head:rotate('y',-90)
head_joint:add_child(head)

nose = gr.sphere('nose')
nose:set_material(red)
nose:translate(0,0,1)
nose:scale(0.5,0.5,0.5)
head:add_child(nose)

--left up arm
left_up_arm_joint = gr.joint('left_up_arm_joint',{-30,0,30},{-30,0,30})
shoulder:add_child(left_up_arm_joint)

left_up_arm = gr.sphere('left_up_arm')
left_up_arm:set_material(white)
--left_up_arm:rotate('z',90)	
left_up_arm:scale(0.5,1.2,0.5)
left_up_arm:translate(-2,-1,0)
left_up_arm_joint:add_child(left_up_arm)

--Right arm
right_up_arm_joint = gr.joint('right_up_arm_joint',{-30,0,30},{-30,0,30})
shoulder:add_child(right_up_arm_joint)

right_up_arm = gr.sphere('right_up_arm')
right_up_arm:set_material(white)
--left_up_arm:rotate('z',90)	
right_up_arm:scale(0.5,1.2,0.5)
right_up_arm:translate(2,-1,0)
right_up_arm_joint:add_child(right_up_arm)

--left for arm
left_for_arm_joint = gr.joint('left_for_arm_joint',{-30,0,30},{-30,0,30})
left_up_arm:add_child(left_for_arm_joint)

left_for_arm = gr.sphere('left_for_arm')
left_for_arm:set_material(white)
--left_up_arm:rotate('z',90)	
left_for_arm:scale(0.5,1,0.5)
left_for_arm:translate(0,-2,0)
left_for_arm_joint:add_child(left_for_arm)

--right for arm
right_for_arm_joint = gr.joint('right_for_arm_joint',{-30,0,30},{-30,0,30})
right_up_arm:add_child(right_for_arm_joint)

right_for_arm = gr.sphere('right_for_arm')
right_for_arm:set_material(white)
--left_up_arm:rotate('z',90)	
right_for_arm:scale(0.5,1,0.5)
right_for_arm:translate(0,-2,0)
right_for_arm_joint:add_child(right_for_arm)

--left hand
left_hand_joint = gr.joint('left_hand_joint',{-30,0,30},{-30,0,30})
left_for_arm:add_child(left_hand_joint)

left_hand = gr.sphere('left_hand')
left_hand:set_material(white)
--left_up_arm:rotate('z',90)	
left_hand:scale(0.7,0.7,0.5)
left_hand:translate(0,-1.5,0)
left_hand_joint:add_child(left_hand)

--right hand
right_hand_joint = gr.joint('right_hand_joint',{-30,0,30},{-30,0,30})
right_for_arm:add_child(right_hand_joint)

right_hand = gr.sphere('right_hand')
right_hand:set_material(white)
--left_up_arm:rotate('z',90)	
right_hand:scale(0.7,0.7,0.5)
right_hand:translate(0,-1.5,0)
right_hand_joint:add_child(right_hand)

--hip
hip = gr.sphere('hip')
hip:set_material(white)
hip:scale(2,0.5,1)
hip:translate(0,-2,0)
--rootnode:add_child(shoulder)
body:add_child(hip)

--left thigh
left_thigh_joint = gr.joint('left_thigh_joint',{-30,0,30},{-30,0,30})
hip:add_child(left_thigh_joint)

left_thigh = gr.sphere('left_thigh')
left_thigh:set_material(white)
--left_up_arm:rotate('z',90)	
left_thigh:scale(0.5,1.5,0.5)
left_thigh:translate(-1,-1.5,0)
left_thigh_joint:add_child(left_thigh)

--Right thight
right_thigh_joint = gr.joint('right_thigh_joint',{-30,0,30},{-30,0,30})
hip:add_child(right_thigh_joint)

right_thigh = gr.sphere('right_thigh')
right_thigh:set_material(white)
--left_up_arm:rotate('z',90)	
right_thigh:scale(0.5,1.5,0.5)
right_thigh:translate(1,-1.5,0)
right_thigh_joint:add_child(right_thigh)

--left calf
left_calf_joint = gr.joint('left_calf_joint',{-30,0,30},{-30,0,30})
left_thigh:add_child(left_calf_joint)

left_calf = gr.sphere('left_calf')
left_calf:set_material(white)
--left_up_arm:rotate('z',90)	
left_calf:scale(0.5,1.0,0.5)
left_calf:translate(0,-1.5,0)
left_calf_joint:add_child(left_calf)

--Right calf
right_calf_joint = gr.joint('right_calf_joint',{-30,0,30},{-30,0,30})
right_thigh:add_child(right_calf_joint)

right_calf = gr.sphere('right_calf')
right_calf:set_material(white)
--left_up_arm:rotate('z',90)	
right_calf:scale(0.5,1,0.5)
right_calf:translate(0,-1.5,0)
right_calf_joint:add_child(right_calf)

--left foot
left_foot_joint = gr.joint('left_foot_joint',{-30,0,30},{-30,0,30})
left_calf:add_child(left_foot_joint)

left_foot = gr.sphere('left_foot')
left_foot:set_material(white)
--left_up_arm:rotate('z',90)	
left_foot:scale(0.7	,0.5,0.5)
left_foot:translate(-0.5,-1,0)
left_foot_joint:add_child(left_foot)

--Right foot
right_foot_joint = gr.joint('right_foot_joint',{-30,0,30},{-30,0,30})
right_calf:add_child(right_foot_joint)

right_foot = gr.sphere('right_foot')
right_foot:set_material(white)
--left_up_arm:rotate('z',90)	
right_foot:scale(	0.7,0.5,0.5)
right_foot:translate(0.5,-1,0)
right_foot_joint:add_child(right_foot)

rootnode:translate(0, 0, -20.0)
return rootnode
