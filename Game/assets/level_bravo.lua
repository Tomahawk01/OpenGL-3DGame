PLAYING = 0
COMPLETE = 1
LOST = 2

barrels = {}
last_player_position = vec3(0.0, 0.0, 0.0)
original_last_player_position = vec3(0.0, 0.0, 0.0)
current_level_state = PLAYING

function init_level(player_position)
	original_last_player_position = player_position
	restart_level()
end

function update_level(player_position)
	if current_level_state ~= PLAYING then
		return
	end

	if barrels[2].visibility then
		barrels[2].position = barrels[2].position + (last_player_position - player_position)
	end
	last_player_position = player_position

	if distance(barrels[1].position, barrels[2].position) < 1.0 then
		current_level_state = COMPLETE
	end
end

function restart_level()
	barrels[1] = {
		position = vec3(0.0, 0.0, 0.0),
		visibility = true,
		color = vec3(0.0, 0.0, 1.0),
		tint = 1.0,
		collision_layer = 1,
		collision_mask = 0,
	}
	barrels[2] = {
		position = vec3(5.0, 0.0, 0.0),
		visibility = true,
		color = vec3(0.0, 0.0, 1.0),
		tint = 0.5,
		collision_layer = 2,
		collision_mask = 2,
	}
	last_player_position = original_last_player_position
	current_level_state = PLAYING
end

function level_state()
	return current_level_state
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

function handle_entity_intersect(index_a, index_b) end
