table.tostring = function(table)
    local next = next
    if next(table) == nil then
        return "{ }"
    end
    local result = "{ "
    for k, v in pairs(table) do
        result = result .. "'" .. tostring(k) .. "': '" .. tostring(v) .. "', "
    end
    result = string.sub(result, 1, string.len(result) - 2)
    return result .. " }"
end
