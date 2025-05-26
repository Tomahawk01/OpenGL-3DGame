barrels = {}
last_player_position = vec3(0.0, 0.0, 0.0)
original_player_position = vec3(0.0, 0.0, 0.0)

function init_level(player_position)
	original_player_position = player_position
	restart_level()
end

function update_level(player_position)
	if barrels[2].visibility then
		barrels[2].position = barrels[2].position + (last_player_position - player_position)
	end
	last_player_position = player_position
end

function restart_level()
	barrels[1] = {
		position = vec3(0.0, 0.0, 0.0),
		visibility = true
	}
	barrels[2] = {
		position = vec3(5.0, 0.0, 0.0),
		visibility = true
	}
	last_player_position = original_player_position
end

function is_complete()
	return distance(barrels[1].position, barrels[2].position) < 1.0
end

function barrel_count()
	return #barrels
end

function barrel_position(index)
	return barrels[index].position
end

function set_barrel_visibility(index, visibility)
	barrels[index].visibility = visibility
end
