barrels = {}
last_player_position = vec3(0.0, 0.0, 0.0)
original_player_position = vec3(0.0, 0.0, 0.0)

function init_level(player_position)
	original_player_position = player_position
	restart_level()
end

function update_level(player_position)
	if barrels[2].visibility then
		barrels[2].position = barrels[2].position + (player_position - last_player_position)
	end
    if barrels[4].visibility then
		barrels[4].position = barrels[4].position + (last_player_position - player_position)
	end
	last_player_position = player_position
end

function restart_level()
	barrels[1] = {
		position = vec3(0.0, 0.0, 5.0),
		visibility = true,
		color = vec3(1.0, 0.0, 0.0),
		tint = 1.0
	}
	barrels[2] = {
		position = vec3(5.0, 0.0, 0.0),
		visibility = true,
		color = vec3(1.0, 0.0, 0.0),
		tint = 0.5
	}
    barrels[3] = {
		position = vec3(0.0, 0.0, -5.0),
		visibility = true,
		color = vec3(0.0, 0.0, 1.0),
		tint = 1.0
	}
	barrels[4] = {
		position = vec3(-5.0, 0.0, 0.0),
		visibility = true,
		color = vec3(0.0, 0.0, 1.0),
		tint = 0.5
	}
	last_player_position = original_player_position
end

function is_complete()
	return distance(barrels[1].position, barrels[2].position) < 1.0 and
           distance(barrels[3].position, barrels[4].position) < 1.0
end

function barrel_count()
	return #barrels
end

function barrel_info(index)
	return barrels[index].position, barrels[index].color, barrels[index].tint
end

function set_barrel_visibility(index, visibility)
	barrels[index].visibility = visibility
end

function set_barrel_position(index, position)
	barrels[index].position = position
end
