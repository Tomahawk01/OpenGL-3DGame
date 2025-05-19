barrels = {}

function init_level()
	print("init_level")

	barrels[1] = vec3(0.0, 0.0, 0.0)
end

function update_level(pos)
	print("update_level ", tostring(pos))
end

function restart_level()
	print("restart_level")
end

function barrel_count()
	return #barrels
end

function barrel_position(index)
	return barrels[index]
end
