barrels = {}
last_player_position = vec3(0.0, 0.0, 0.0)
original_last_player_position = vec3(0.0, 0.0, 0.0)

function init_level(player_position)
	original_last_player_position = player_position
	restart_level()
end

function update_level(player_position)
	if not barrels[2].visibility then
		barrels[2].position = barrels[2].position + vec3(0.1, 0.0, 0.0)
	else
		local delta = player_position - last_player_position
		if delta.x > 0.0 then
			delta.x = 0.0
		end
		barrels[2].position = barrels[2].position + delta
	end
	last_player_position = player_position
end

function restart_level()
	barrels[1] = {
		position = vec3(5.0, 0.0, 20.0),
		visibility = true,
		color = vec3(0.0, 1.0, 0.0),
		tint = 1.0,
		collision_layer = 1,
		collision_mask = 0,
	}
	barrels[2] = {
		position = vec3(-5.0, 0.0, 0.0),
		visibility = true,
		color = vec3(0.0, 1.0, 0.0),
		tint = 0.5,
		collision_layer = 2,
		collision_mask = 2,
	}
	last_player_position = original_last_player_position
end

function is_complete()
	return distance(barrels[1].position, barrels[2].position) < 1.0
end

function barrel_count()
	return #barrels
end

function barrel_info(index)
	return barrels[index].position,
		   barrels[index].color,
		   barrels[index].tint,
		   barrels[index].collision_layer,
		   barrels[index].collision_mask
end

function set_barrel_visibility(index, visibility)
	barrels[index].visibility = visibility
end

function set_barrel_position(index, position)
	barrels[index].position = position
end
