function Extensions:index(path)
    return 200, "<!doctype html><html><head><title>Placeholder</title></head><body>" .. table.tostring(self) .. "</body></html>"
end