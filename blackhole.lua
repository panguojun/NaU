require 'com'
mod(1)
psz(4)


------------------------------------------------
c_scale = 0.2
ISCO = 3
ICO = 1.5

bh = {R = 2}
dt = 1
------------------------------------------------
function PIX(p)	
	psz(4)
	pix(p.x * 0.01, p.y * 0.01, 20 + p.z * 0.5)
end
function PIX0(p)	
	psz(1)
	pix(p.x * 0.005, p.y * 0.005, 20)
end
function CIRCLE(x0, y0, r)	
	for i = 0, 100 do
		ang = i / 100 * 3.1416 * 2
		x = x0 + r * math.cos(ang)
		y = y0 + r * math.sin(ang)
		PIX({x = x, y = y, z = 0})
	end
end
function GR(bh, p)
	return math.sqrt(1 - bh.R / p.r)
end
------------------------------------------------
for k = -300, 300 do
	for j = -300, 300 do	
		local p = {t = 0, x = j * c_scale, y = k * c_scale, z = -100}
		local s = RAF(p)
		local dp = {dt = 0, dx = 0, dy=-0.025, dz = 1, vx = 0, vy = -0.025, vz = 1}	
		local cor = 0
		for i = 1, 600 do
			if s.r > bh.R then
				s = RAF(p)
			
				local n = 1 / (1 - bh.R/s.r)
				local dvx = -n*n *bh.R/(s.r*s.r*s.r) * p.x * dt
				local dvy = -n*n *bh.R/(s.r*s.r*s.r) * p.y * dt
				local dvz = -n*n *bh.R/(s.r*s.r*s.r) * p.z * dt
				
				dp.vx = dp.vx + dvx
				dp.vy = dp.vy + dvy
				dp.vz = dp.vz + dvz
				
				dp.dx = dp.vx / n * dt
				dp.dy = dp.vy / n * dt
				dp.dz = dp.vz / n * dt
				
				ADD(p, dp)
				
				if p.y < 0 and p.y > -dt/2 then
					if s.r > bh.R * ISCO and s.r < bh.R * 25 then
						
						cor = bnd(250, 50, math.sin(s.r * 1))
						break
						
					end
				end
			else
				cor = 0
				break
			end
		end
		
		rgb(cor, cor, cor)
		PIX({t = 0, x = j * c_scale, y = k * c_scale, z = 0})
	end
end
------------------------------------------------
--[[
local dt = 0.5
rgb(250, 250, 250)
for j = 0, 400 do	
	local p = {t = 0, x = -bh.R * 20, y = j * 0.08, z = 0}
	local dp = {dt = 0, dx = dt, dy = 0, dz = 0, vx = dt, vy = 0, vz = 0}	
	for k = 0, 1000 do
	
		local s = RAF(p)
	
		local n = 1 / (1 - bh.R/s.r)
		local dvx = -n*n *bh.R/(s.r*s.r*s.r) * p.x * dt
		local dvy = -n*n *bh.R/(s.r*s.r*s.r) * p.y * dt
		local dvz = -n*n *bh.R/(s.r*s.r*s.r) * p.z * dt
		
		dp.vx = dp.vx + dvx
		dp.vy = dp.vy + dvy
		dp.vz = dp.vz + dvz
		
		dp.dx = dp.vx / n * dt
		dp.dy = dp.vy / n * dt
		dp.dz = dp.vz / n * dt
		
		ADD(p, dp)
		
		--pnt(p.x)
		
		if s.r > bh.R then
			PIX({t = 0, x = p.x , y = p.y, z = 0})
			--pnt("p.x " .. p.x)
			--pnt("p.y " .. p.y)
			--if p.y < 0 and p.y > -1 then
				--if s.r > bh.R * ISCO and s.r < bh.R * 100 then
				--	break
				--end
			--end
		else
			break
		end
	end
end	

psz(1)
rgb(250, 0, 0)
CIRCLE(0,0,bh.R)

rgb(250, 250, 0)
CIRCLE(0,0,bh.R * ICO)

rgb(250, 0, 250)
CIRCLE(0,0,bh.R * ISCO)

--]]
