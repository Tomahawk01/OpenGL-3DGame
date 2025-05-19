barrels = {}
last_player_position = vec3(0.0, 0.0, 0.0)

function init_level(player_position)
	print("init_level")

	barrels[1] = vec3(0.0, 0.0, 0.0)
	barrels[2] = vec3(5.0, 0.0, 0.0)
	last_player_position = player_position
end

function update_level(player_position)
	print("update_level ", tostring(player_position))

	barrels[2] = barrels[2] + (player_position - last_player_position)
	last_player_position = player_position
end

function restart_level()
	print("restart_level")
end

function is_complete()
	return distance(barrels[1], barrels[2]) < 0.1
end

function barrel_count()
	return #barrels
end

function barrel_position(index)
	return barrels[index]
end
