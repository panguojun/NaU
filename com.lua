PI = math.pi
------ lua ------
function clone(object)
     local lookup_table = {}
    local function copyObj( object )
        if type( object ) ~= "table" then
            return object
        elseif lookup_table[object] then
            return lookup_table[object]
        end
        
        local new_table = {}
        lookup_table[object] = new_table
        for key, value in pairs( object ) do
            new_table[copyObj( key )] = copyObj( value )
        end
        return setmetatable( new_table, getmetatable( object ) )
    end
    return copyObj( object )
end
------ math ------
function blend(a, b, alpha)
	return a * (1 - alpha) + b * alpha
end
function d6()
	return math.ceil(math.random() * 5 + 1);
end
function d100()
	return math.ceil(math.random() * 99 + 1);
end
function dx(x)
	return math.ceil(math.random() * (x - 1) + 1);
end
function c6()
	if math.ceil(math.random() * 6) == 0 then
		return 1;
	end
	return 0;
end
function rnd(min, max)
	return math.random() * (max - min) + min;
end
function rndi(min, max)
  min = math.ceil(min);
  max = math.floor(max);
  return math.floor(math.random() * (max - min)) + min;
end
function parity(a)
	n, dec = math.modf(a, 2)
	if n == 0 then
		return 1
	else
		return -1
	end
end
----------------------------
--			3D
----------------------------
function dot(v1, v2)
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z
end

-- 向量叉乘
function cross(v1, v2)
    local v3 ={x = v1.y*v2.z - v2.y*v1.z , y = v2.x*v1.z-v1.x*v2.z , z = v1.x*v2.y-v2.x*v1.y}
    return v3
end

-- 向量的模
function length(v)
    return math.sqrt(v.x * v.x + v.y * v.y + v.z * v.z)
end

-- 求两向量间夹角
function angle(v1, v2)
    local cos = MathHelper.GetVector3Dot(v1, v2)/ (MathHelper.GetVector3Module(v1)*MathHelper.GetVector3Module(v2))
    return math.acos(cos) * 180 / math.pi
end

------ Topology ------
function section(sig, fun)
	for i = 1, sig do
		push()
		pushc()
		fun(i)
	end
	--pop(sig)
	--popc(sig)
end
function branch2(d, dv, fun1, fun2)
	push()
	ext(d)
	scl(1.5)
	wdiv(dv, 1, 0)
	scl(0.25)
	face()
		div(0, dv)
		cls()
			double()
			push()
			pushc()
			fun1()
			popc()
			pop()
		pop()
		cls()
			double()
			push()
			pushc()
			fun2()
			popc()
			pop()
		pop()
	pop()
end
function seg()
	push()
	pushc()
	wgt()
end
function sed(n)
	popc()
	if n == nil then
		pop()
	else
		pop(n)
	end	
end
----------------------------
--		Draw 2D
----------------------------
function circle(x0, y0, r)	
	for i = 0, 200 do
		ang = i / 100 * 3.1416 * 2
		x = x0 + r * math.cos(ang)
		y = y0 + r * math.sin(ang)
		pix(x, y)
	end
end
function line(x1, y1, x2, y2)	
	for i = 0, 100 do
		pix(blend(x1, x2, i / 100), blend(y1, y2, i / 100))
	end
end
----------------------------
--		Astrophysics
----------------------------
function SQRLEN(v)
    return (v.x * v.x + v.y * v.y)
end
function LEN(v)
    return math.sqrt(v.x * v.x + v.y * v.y)
end
function DIS(p1, p2)
	local v = {x=p2.x-p1.x,y=p2.y-p1.y}
    return math.sqrt(v.x * v.x + v.y * v.y)
end
function VEC(p1, p2)
	local v = {x=p2.x-p1.x,y=p2.y-p1.y}
	local r = LEN(v)
	v.x = v.x / r
	v.y = v.y / r
	return v
end
function MUL(p, s)
	local v={x=0,y=0}
	v.x = p.x * s
	v.y = p.y * s
	return v
end
function ADD(p1, p2)
	local v={x=0,y=0}
	v.x = p1.x + p2.x
	v.y = p1.y + p2.y
	return v
end
function SUB(p1, p2)
	local v={x=0,y=0}
	v.x = p1.x - p2.x
	v.y = p1.y - p2.y
	return v
end
----------------------------
ops={}
function V(v)
	setmetatable(v, ops)
	return v
end
ops.__eq=function (a, b)
	if a.x == b.x and a.y == b.y then
		return true
	end
	return false
end
ops.__tostring=function (a)
	return("x=" .. a.x .. "y=" .. a.y)
end
ops.__add=function (a, b)
	local v={x=0,y=0}
	v.x = a.x + b.x
	v.y = a.y + b.y
	return v
end
ops.__sub=function(p1, p2)
	local v={x=0,y=0}
	v.x = p1.x - p2.x
	v.y = p1.y - p2.y
	return v
end
ops.__mul=function(p, s)
	local v={x=0,y=0}
	v.x = p.x * s
	v.y = p.y * s
	return v
end
-----------edge-------------
eops={}
eops.__add=function(e,a)
	table.insert(e,a)
	return e
end
eops.__tostring=function (e)
	local s = '\ne:\n'
	for i=1,#e do
		s = s .. ("x=" .. e[i].x .. ",y=" .. e[i].y)
		s = s .. '\n'
	end
	return s
end
function E()
	local e = {}
	setmetatable(e, eops)
	return e
end
---------3D DRAW----------
function TRIANG(p1,p2,p3)
	triang(p1.x,p1.y,0,p2.x,p2.y,0,p3.x,p3.y,0)
end
function FACE(e1, e2)
	newe()
	for i=1,#e1 do
		addv(e1[i].x,e1[i].y,0)
	end
	newe()
	for i=1,#e2 do
		addv(e2[i].x,e2[i].y,1)
	end
	face()
	pop()
	pop()
end
--[[
e = E()
e = e + V({x=0.1,y=0.2})
e = e + V({x=0.2,y=0.3})
e = e + V({x=0.5,y=0.1})
e1 = E()
e1 = e1 + V({x=0.8,y=0.2})
e1 = e1 + V({x=0.7,y=0.3})
e1 = e1 + V({x=0.9,y=0.1})
FACE(e, e1)
print(e)
--]]

