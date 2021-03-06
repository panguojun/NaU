#include "com.h"
// ################################################################################################
vec CAM_POS = vec::CENTER;
inline void viewprj(const vector3& rp, vector2& p2d, real& depth)
{
	//vec p = rp;
	//p -= CAM_POS;
	//p.rot(PI * 0.25, vec::UY);
	//p.rot(PI * 0.15, vec::UX);
	//p += CAM_POS;
	//p.z += MAXZ / 3;

	real z = rp.z;
	depth = z;
	real vpf = blend(1, 0.01, z / 100.0f);
	p2d.x = 0.5f + rp.x * vpf;
	p2d.y = 0.5f + rp.y * vpf;
}
// -------------------------------------------------
inline real getviewdis(const vec& p1, const vec& p2)
{
	real depth;
	vec2 pp1, pp2;
	viewprj(p1, pp1, depth);
	viewprj(p2, pp2, depth);
	return (pp1 - pp2).len() * (IMAGESCALE);
}
// -------------------------------------------------
double RidgedMFex(double x, double y, double z, int octaves, float offset)
{
	double sum = 0;
	float amplitude = 0.5f;
	float frequency = 1.0f;
	double prev = 1.0f;
	
	double gain = 0.50f;
	double lacunarity = 2.5f;
	
	for (int i = 0; i < octaves; i++)
	{
		double n = ridge(perlin(x * frequency, y * frequency, z * frequency), offset);		
		sum += n * amplitude * prev;
		prev = n;
		frequency *= lacunarity;
		amplitude *= gain;
	}

	return sum;
}
// -------------------------------------------------
vec gworldpos;
real fixedSZ(real x)
{	
	return int((x) / USIZE + 0.5) * USIZE;
}
real fixedSZfloor(real x)
{
	return int((x) / USIZE) * USIZE;
}

real fract(real v)
{
	return v - int(v);
}
real rndbypos2(const vec2& pos, real min = 0, real max = 1)
{
	float h = vec2(pos.x, pos.y).dot(vec2(127.1, 311.7));
	return blend(min, max, fract(sin(h) * 43758.5453123));
}
real rndbypos(crvec pos, real min = 0, real max = 1)
{
	float h = vec2(pos.x, pos.z).dot(vec2(127.1, 311.7));
	return blend(min, max, fract(sin(h + fract(pos.y)) * 43758.5453123));
}
real rndbypos(real min = 0, real max = 1)
{
	float h = vec2(gworldpos.x, gworldpos.z + gworldpos.y).dot(vec2(127.1, 311.7));
	return blend(min, max, fract(sin(h) * 43758.5453123));
}
real rndbypos(real x, real y, real min = 0, real max = 1)
{
	float h = vec2(x, y).dot(vec2(127.1, 311.7));
	return blend(min, max, fract(sin(h) * 43758.5453123));
}
// -------------------------------------------------
// 得到向量的两个垂直方向
// -------------------------------------------------
void v2vxvy(const vec& v, vec& vx, vec& vy)
{
	if (gIsSetDxDy)
	{
		vx = gDX; vy = gDY;
		return;
	}
	vec uz = vec::UZ;
	if (v.x < EPSINON && v.x > -EPSINON && v.y < EPSINON && v.y > -EPSINON)
	{
		uz = vec::UX;		
	}
	vx = uz.cross(v);
	vy = v.cross(vx); vy.norm();
	
}
// -------------------------------------------------
inline vec scaleby(crvec v, crvec o, real s)
{
	return (v - o) * s + o;
}

// ================================================================================================
// 绘制
// 三角形元素
// 渲染光栅化
// ================================================================================================
// 渲染状态
vec light(0.5, 0.5, -0.5);		// 全局光的反向
int color = 0xFF808080;		// 当前色彩
int lummode = 0;			// -1 暗 0 正常 1 亮
bool backfacecull = false;	// 背面剔除
int renderstate = 0;		// 渲染模式 0 不渲染， 1 正常渲染 2 网格渲染
bool binvnorm = false;
bool bclosed = true;
// ------------------------------------------------------------------------------------------------
void triang(vertex& p1, vertex& p2, vertex& p3)
{
	if (renderstate == 0)
	{
		if (gcalcnorm)
		{
			vec n = binvnorm ? (p3 - p1).cross(p2 - p1) : (p2 - p1).cross(p3 - p1);
			n.norm();
			if (p1.n == vec::ZERO) p1.n = n;
			if (p2.n == vec::ZERO) p2.n = n;
			if (p3.n == vec::ZERO) p3.n = n;
		}
		if (binvnorm)
			addtri2sm(p1, p2, p3);
		else
			addtri2sm(p1, p3, p2);
		return;
	}

	int cor = 0xFFFFFFFF;
	if (renderstate == 1)
	{
		vec norm = binvnorm ? (p3 - p1).cross(p2 - p1) : (p2 - p1).cross(p3 - p1);
		if (norm.sqrlen() == 0)
		{
			//pixelAA((p1 + p2 + p3) / 3.0, cor);
			//line3d(p1, p2, cor);
			//line3d(p1, p3, cor);
			//line3d(p2, p3, cor);
			return; // 无效三角形
		}
		norm.norm();
		if (backfacecull && norm.z > 0)
			return;
		
		if (color == 0xFFFFFFFF)
		{
			int cor0 = 0x000000FF & cor;
			if (lummode == 1)
			{
				int lum = blend(cor0, 255, norm.dot(light) * 1.5, 3);
				cor = RGB(lum, lum, lum);
			}
			else if (lummode == -1)
			{
				int lum = blend(cor0, 188, norm.dot(light), 2);
				cor = RGB(lum, lum, lum);
			}
			else
			{
				int lum = blend(cor0, 255, norm.dot(light));
				cor = RGB(lum, lum, lum);
			}
		}
		else
		{
			int lum = blend(0, 255, norm.dot(light) * 1.5, 3);
			cor = RGB(lum, lum, lum);
			cor = blendcor(cor, color, 0.5);
		}

		LOOP0(i, len, getviewdis(p1.p, p2.p))
			vec p12 = blend(p1.p, p2.p, ai);
		LOOP0(j, wid, getviewdis(p12, p3.p))
				vec p = blend(p12, p3.p, aj);
				pixel(p, cor);
			END
		END

	}
	else if (renderstate == 2)
	{
		line3d(p1.p * DRAWSCALE, p2.p * DRAWSCALE, cor);
		line3d(p2.p * DRAWSCALE, p3.p * DRAWSCALE, cor);
		line3d(p3.p * DRAWSCALE, p1.p * DRAWSCALE, cor);
	}
}

void triang0(const vertex& p1, const vertex& p2, const vertex& p3)
{
	if (renderstate == 0)
	{
		if (binvnorm)
			addtri2sm0(p1, p2, p3);
		else
			addtri2sm0(p1, p3, p2);
		return;
	}

	int cor = 0xFFFFFFFF;
	if (renderstate == 1)
	{
		 vec norm = binvnorm ? (p3 - p1).cross(p2 - p1) : (p2 - p1).cross(p3 - p1);
		 if (norm.sqrlen() == 0)
		 {
		 	//pixelAA((p1 + p2 + p3) / 3.0, cor);
		 	//line3d(p1, p2, cor);
		 	//line3d(p1, p3, cor);
		 	//line3d(p2, p3, cor);
		 	return; // 无效三角形
		 }
		
		 if (backfacecull && norm.z > 0)
		 	return;
		{
			unsigned char lum = blendi(0, 255, norm.dot(light) * 1.5);
			cor =  _RGB(lum, lum, lum);
			cor = blendcor(cor, color, 0.5);
		}
		{
			LOOP0(i, len, getviewdis(p1.p, p2.p))
				vec p12 = blend(p1.p, p2.p, ai);
			LOOP0(j, wid, getviewdis(p12, p3.p))
				vec p = blend(p12, p3.p, aj);
			pixel(p,cor);
			END
				END
		}
	}
	else if (renderstate == 2)
	{

		vec norm = (p2 - p1).cross(p3 - p1);
		if (norm.sqrlen() == 0)
		{
			//pixelAA((p1 + p2 + p3) / 3.0, cor);
			//line3d(p1, p2, cor);
			//line3d(p1, p3, cor);
			//line3d(p2, p3, cor);
			return; // 无效三角形
		}
		
		line3d(p1.p, p2.p, color);
		line3d(p2.p, p3.p, color);
		line3d(p3.p, p1.p, color);
	}
}

// -------------------------------------------------
// 面
// -------------------------------------------------
void plane(vertex& p1, vertex& p2, vertex& p3, vertex& p4)
{
	triang(p2, p3, p1);
	triang(p4, p1, p3);
}
void plane0(const vertex& p1, const vertex& p2, const vertex& p3, const vertex& p4)
{
    triang0(p2, p3, p1);
    triang0(p4, p1, p3);
}
// -------------------------------------------------
// 边缘->面
// -------------------------------------------------
int drawmask = 0xFFFFFFFF;
inline int checkflag(int i)
{
	return drawmask & (1 << i);
}
inline int unflag(int i)
{
	return drawmask &= ~(1 << i);
}
void face( VECLIST& edge, bool invnorm = false)
{	
	bool oldbinvnorm = binvnorm; 
	binvnorm = invnorm;
	int cor = RNDBW;
	int s = edge.size();
	if(s < 3)
		return;
	if (s == 5)
	{
		plane(edge[0], edge[1], edge[2], edge[3]);
	}
	else
	{
		vertex p0 = edge[0];
		for (int i = 1; i < s - 1; i++)
		{
			if (checkflag(i))
				triang(p0, edge[i], edge[(i + 1) % s]);
		}
	}
	drawmask = 0xFFFFFFFF;
	binvnorm = oldbinvnorm;
}
void face( VECLIST& edge, vertex o)
{	
	int cor = RNDBW;
	int s = edge.size();
	if(s < 3)
		return;	
	for(int i = 0; i < s - 1; i ++)
	{		
		if(checkflag(i))
        {
            triang(o, edge[i], edge[(i + 1) % s]);
        }
	}
	drawmask = 0xFFFFFFFF;
}
// 极点
void facepole(VECLIST& edge, vertex o)
{
	int cor = RNDBW;
	int s = edge.size();
	if (s < 3)
		return;
	for (int i = 0; i < s - 1; i++)
	{
		vertex& v1 = edge[i];
		vertex& v2 = edge[(i + 1) % s];
		o.uv.x = (v1.uv.x + v2.uv.x) / 2;

		triang(o, v1, v2);
	}
}
void face(VECLIST& edge1, VECLIST& edge2)
{
	int len = _MIN(edge1.size(), edge2.size());
	len = int(len - 1);
	for (int i = 0; i < len; i++)
	{
		plane(edge1[i], edge1[i + 1], edge2[i + 1], edge2[i]);
		if (i == 0 && gcommonvertex && bclosed)
		{
			edge1[len].ind = edge1[0].ind;
			edge2[len].ind = edge2[0].ind;
		}
	}
	
	drawmask = 0xFFFFFFFF;
}
void face0(VECLIST& edge1, VECLIST& edge2)
{
	int len = int(edge1.size() - 1);
	for (int i = 0; i < len; i++)
	{
		plane(edge1[i], edge1[i + 1], edge2[i + 1], edge2[i]);
	}
}
void face12(VECLIST& edge1, VECLIST& edge2)
{
	int len = int(edge1.size() - 1);
	for (int i = 0; i < len; i++)
	{
		if (checkflag(i))
		{
			plane(edge1[i], edge1[i + 1], edge2[i * 2 + 1], edge2[i * 2]);
			triang(edge1[i + 1], edge2[i * 2 + 2], edge2[i * 2 + 1]);
		}
		if (i == 0 && gcommonvertex)
		{
			edge1[len].ind = edge1[0].ind;
			edge2[int(edge2.size() - 1)].ind = edge2[0].ind;
		}
	}
	drawmask = 0xFFFFFFFF;
}

// -------------------------------------------------
// 边逻辑
// -------------------------------------------------
VECLIST* pworkingedge = 0; 	// 当前工作面
int workingedgeindex = 0;	 		// 当前工作面索引

// -------------------------------------------------
// 边法线方向
inline vec getedgenorm(const VECLIST& e)
{
	if(e.size() < 3)
		return vec::ZERO;
	{
		int ind1 = 1;
		for (; ind1 < e.size(); ind1++)
		{
			if (e[ind1].p != e[0].p)
				break;
		}
		int ind2 = ind1 + 1;
		for (; ind2 < e.size(); ind2++)
		{
			if (e[ind2].p != e[ind1].p && e[ind2].p != e[0].p)
				break;
		}
		if (ind2 >= e.size())
			return vec::ZERO;
		return (e[ind1].p - e[0].p).cross(e[ind2].p - e[0].p).normcopy();
	}
}
// 平均点
vec getedgecenter(const VECLIST& e, int n = 0)
{
    if(n == 0)
        n = (e[0].p == e[e.size() - 1].p) ? e.size() - 1 : e.size();
	vec o;
	for(int i = 0; i < n; i ++)
		o = o + e[i].p;
	o = o / n;
	return o;
}
// -------------------------------------------------
// 边缘大小
void radedge(VECLIST& e1, real r, VECLIST& e2)
{
    int len = e1.size();
    if(len <= 1)
        return;
    if(e1[0] == e1[len - 1])
        len --;
    vec o;
    for(int i = 0; i < len; i ++)
        o = o + e1[i];
    o = o / len; // 平均点
    
    for(int i = 0; i < e1.size(); i ++)
    {
        vertex v = e1[i];
        v.p = ((v.p - o).normcopy() * r + o);
        e2.PUSH(v);
    }
}
void radedge(VECLIST& e1, real r)
{
    int len = e1.size();
    if(len <= 1)
        return;
    if(e1[0] == e1[len - 1])
        len --;
    vec o;
    for(int i = 0; i < len; i ++)
        o = o + e1[i];
    o = o / len; // 平均点
    
    for(int i = 0; i < e1.size(); i ++)
    {
        vertex& v = e1[i];
        v.p = ((v.p - o).normcopy() * r + o);
    }
}
void radedge2(VECLIST& e1, real r)
{
	int len = e1.size();
	if (len <= 1)
		return;
	if (e1[0] == e1[len - 1])
		len--;
	vec o;
	for (int i = 0; i < len; i++)
		o = o + e1[i];
	o = o / len; // 平均点

	vec n = getedgenorm(e1);
	vec v0 = (e1[0] - o).normcopy();
	real ang = 2 * PI / len;
	for (int i = 0; i < e1.size(); i++)
	{
		vertex& v = e1[i];
		v.p = (v0 * r + o);
		v0.rot(ang, n);
	}
}
// -------------------------------------------------
// 缩放边缘
void scaleedge(const VECLIST& e1, real scale, VECLIST& e2)
{
	int len = e1.size();
	if(len <= 1)
		return;
	if(e1[0].p == e1[len - 1].p)
		len --;		
	vec o;
	for(int i = 0; i < len; i ++)
		o = o + e1[i].p;
	o = o / len; // 平均点
	
	for(int i = 0; i < e1.size(); i ++)
	{
		vertex v = e1[i];
		v.p = (v.p - o) * scale + o;
		e2.PUSH(v);
	}
}
// -------------------------------------------------
void scaleedge(VECLIST& e1, real scale, int oplen = 0)
{
	int len = e1.size();
	if(len <= 1)
		return;
	if(e1[0] == e1[len - 1])
		len --;	
	vec o;
	for(int i = 0; i < len; i ++)
		o = o + e1[i];
	o = o / len; // 平均点
	
    if(oplen == 0)
        oplen = e1.size();
	for(int i = 0; i < oplen; i ++)
	{
		vertex& v = e1[i];
		v.p = ((v.p - o) * scale + o);
	}
}
// -------------------------------------------------
void scaleedge(VECLIST& e1, real sx, real sy, real sz, VECLIST& e2)
{
	if(e1.size() == 0)
		return;
	vec o;
	for(int i = 0; i < e1.size(); i ++)
		o = o + e1[i].p;
	o = o / e1.size(); // 平均点
	
	for(int i = 0; i < e1.size(); i ++)
	{
		vertex v = e1[i];
		v.p -= o;
		v.p.x *= sx;
		v.p.y *= sy;
		v.p.z *= sz;
		v.p += o;
		e2.PUSH(v);
	}
}
// -------------------------------------------------
void scaleedge(VECLIST& e1, real sx, real sy, real sz)
{
	if(e1.size() == 0)
		return;
	vec o;
	for(int i = 0; i < e1.size() - 1; i ++)
		o = o + e1[i];
	o = o / (e1.size() - 1); // 平均点
	
	for(int i = 0; i < e1.size(); i ++)
	{
		vertex& v = e1[i];
		v.p -= o;
		v.p.x *= sx;
		v.p.y *= sy;
		v.p.z *= sz;
		v.p += o;
		v.ind = -1;
	}
}
// -------------------------------------------------
void scaleeedgeex(VECLIST& e1, std::vector<real> scalelist)
{
	if (e1.size() == 0)
		return;
	vec o;
	for (int i = 0; i < e1.size() - 1; i++)
		o = o + e1[i];
	o = o / (e1.size() - 1); // 平均点

	for (int i = 0; i < e1.size(); i++)
	{
		vertex& v = e1[i];
		v.p -= o;
		if (i < scalelist.size())
			v.p *= scalelist[i];

		v.p += o;
	}
}
// -------------------------------------------------
void moveedgeex(VECLIST& e1, std::vector<real> movelist)
{
	if (e1.size() == 0)
		return;
	vec norm = getedgenorm(e1);
	for (int i = 0; i < e1.size(); i++)
	{
		vertex& v = e1[i];
		if (i < movelist.size())
			v.p += norm * movelist[i];
	}
}

// -------------------------------------------------
// 移动边缘
void moveedge(const VECLIST& e1, vec dv, VECLIST& e2)
{
	for(int i = 0; i < e1.size(); i ++)
	{
		vertex v = e1[i];
		v.p += dv;
		e2.PUSH(v);
	}
}
// -------------------------------------------------
void moveedge(VECLIST& e1, vec dv)
{
	for(int i = 0; i < e1.size(); i ++)
	{
		e1[i].p += dv;
		e1[i].ind = -1;
	}
}
void moveedgex(VECLIST& e, vec dv)
{
	VECLIST e1;
	for (int i = 0; i < e.size(); i++)
	{
		e1.PUSH(e[i].p + dv);
	}
	face(e, e1);
	e = e1;
}
// -------------------------------------------------
// extrude 边缘
void extrudeedge(const VECLIST& e1, real d, VECLIST& e2)
{
	if(e1.size() < 3)
		return;
	vec norm = (e1[1].p - e1[0].p).cross(e1[2].p - e1[0].p);norm.norm();
	vec dv = norm * d;
	for(int i = 0; i < e1.size(); i ++)
	{
		vertex v = e1[i];
		v.p += dv;
		e2.PUSH(v);
	}
}
// -------------------------------------------------
void extrudeedge(VECLIST& e1, real d)
{
	if(e1.size() < 3)
		return;
	vec norm = getedgenorm(e1);
	vec dv = norm * d;
	for(int i = 0; i < e1.size(); i ++)
	{
		e1[i].p += dv;
		e1[i].ind = -1;
	}
}

// -------------------------------------------------
// cut 边缘
void cutedge(const VECLIST& ei, int start, int len,  VECLIST& eo)
{
	if(start >= ei.size() || start + len >= ei.size())
		return;
	for(int i = start; i < start + len; i ++)
	{
		eo.PUSH(ei[i]);
	}
}
void cutedge(VECLIST& e, int start, int len)
{
	if(start >= e.size() || start + len >= e.size())
		return;
	int ii = 0;
	for(int i = start; i < start + len; i ++)
	{
		e[ii ++] = e[i];
	}
}
void cutedge(const VECLIST* ei, int pos,  VECLIST* e1 = 0, VECLIST* e2 = 0)
{
	if(!ei || pos >= ei->size())
		return;
	if(e1)
	{
		for(int i = 0; i < pos; i ++)
		{
			e1->PUSH((*ei)[i]);
		}
	}
	if(e2)
	{
		for(int i = pos; i < ei->size(); i ++)
		{
			e2->PUSH((*ei)[i]);
		}
	}
}
// -------------------------------------------------
// subedge 边缘
void subedge(const VECLIST& e, int start, int sublen, VECLIST& se)
{
	int len = e.size();
	for(int i = start; i < start + sublen; i ++)
	{
		int ii = i % len;
		se.PUSH(e[ii]);
	}
}
void subedge2(const VECLIST& e, int start, int sublen, VECLIST& se1, VECLIST& se2)
{
	int len;
	if (bclosed)
		len = (e[0].p == e[e.size() - 1].p) ? e.size() - 1 : e.size();
	else
		len = e.size();
	for (int i = start; i < start + len; i++)
	{
		int ii = i % len;
		if (i >= start && i <= start + sublen)
		{
			se1.PUSH(e[ii]);
			if (i == start + sublen)
				se2.PUSH(e[ii]);
		}
		else
			se2.PUSH(e[ii]);
	}
	//if(isclosed)
		se2.PUSH(e[start % len]);
}

// -------------------------------------------------
// union 边缘
void unionedge(const VECLIST& e1, const VECLIST& e2, int start, int len, VECLIST& eo) // 注意要逆时针方向, e2前, e1后关系
{
	if(len > e1.size() || len > e2.size())
		return;
	
	int sz1 = e1.size();
	for(int i = start; i < start + len; i ++)
	{
		eo.PUSH(e1[i % sz1]);
	}
	int sz2 = e2.size();
	for(int i = start + len - 1; i >= start; i --)
	{
		eo.PUSH(e2[i % sz2]);
	}	
}
// -------------------------------------------------
// 反向
void invedge(VECLIST& e)
{
	VECLIST ne;
	for(int i = e.size() - 1; i >= 0; i --)
	{
		ne.PUSH(e[i]);		
	}
	e = ne;
}
void invplaneedge12(VECLIST& e)
{
	if(e.size() < 4) 
		return;
	vec tmp = e[0];
	e[0] = e[1];
	e[1] = tmp;
}
void invplaneedge34(VECLIST& e)
{
	if(e.size() < 4) 
		return;
	vec tmp = e[2];
	e[2] = e[3];
	e[3] = tmp;
}
void invplaneedge13(VECLIST& e)
{
	if(e.size() < 4) 
		return;
	vec tmp = e[0];
	e[0] = e[2];
	e[2] = tmp;
}
void invplaneedge24(VECLIST& e)
{
	if(e.size() < 4) 
		return;
	vec tmp = e[1];
	e[1] = e[3];
	e[3] = tmp;
}
void invplaneedge23(VECLIST& e)
{
	if(e.size() < 4) 
		return;
	vec tmp = e[1];
	e[1] = e[2];
	e[2] = tmp;
}
void invplaneedge14(VECLIST& e)
{
	if(e.size() < 4) 
		return;
	vec tmp = e[0];
	e[0] = e[3];
	e[3] = tmp;
}
// -------------------------------------------------
// 旋转 边缘
void rotedge0(VECLIST& e, real ang, const vec& ax)
{
	for (int i = 0; i < e.size(); i++)
	{
		e[i].p.rot(ang, ax);
	}
}
void rotedge(VECLIST& e, real ang, const vec& ax)
{
	vec o = getedgecenter(e);
	for(int i = 0; i < e.size(); i ++)
	{
		e[i] = (e[i] - o).rotcopy(ang, ax) + o;
	}
}
void rotedge(const VECLIST& e, real ang, const vec& ax, VECLIST& eo)
{
	vec o = getedgecenter(e);
	for(int i = 0; i < e.size(); i ++)
	{
		eo.PUSH((e[i].p - o).rotcopy(ang, ax) + o);
	}
}
void rotedgebycenter(VECLIST& e, real ang, const vec& ax, const vec& ro)
{
	for (int i = 0; i < e.size(); i++)
	{
		e[i] = (e[i] - ro).rotcopy(ang, ax) + ro;
	}
}
void rotedgebycenter(VECLIST& e, real ang, const vec& ax, const vec& ro, VECLIST& oe)
{
	for (int i = 0; i < e.size(); i++)
	{
		oe.PUSH((e[i].p - ro).rotcopy(ang, ax) + ro);
	}
}
void rotedgebynorm(VECLIST& e, const vec& norm)
{	
	vec n = getedgenorm(e);
	if (n == norm || n == -norm)
		return;

	vec ax = n.cross(norm); ax.norm();
	real ang = acos(norm.dot(n));
	
	rotedge(e, ang, ax);	
}
void rotedgebynorm(const VECLIST& e, const vec& norm, VECLIST& eo)
{
	vec n = getedgenorm(e);
	if (n == norm || n == -norm)
    {
        eo = e;
		return;
    }
	vec ax = n.cross(norm); ax.norm();
	real ang = acos(norm.dot(n));
    vec az = n.cross(ax);
    if(az.dot(norm) > 0)
        ang = - ang;

    rotedge(e, ang, ax, eo);
}
void rotedgebynorm(VECLIST& e, const vec& norm, const vec& ax)
{
	vec n = getedgenorm(e);
	if (n == norm || n == -norm)
		return;

	real ang = acos(norm.dot(n));
    vec az = n.cross(ax);
    if(az.dot(norm) > 0)
        ang = - ang;

	rotedge(e, ang, ax);
}
void rotedgebynorm(const VECLIST& e, const vec& norm, const vec& ax, VECLIST& eo)
{
	vec n = getedgenorm(e);
	if (n == norm || n == -norm)
		return;
	real ang = acos(norm.dot(n));
    vec az = n.cross(ax);
    if(az.dot(norm) > 0)
        ang = - ang;

    rotedge(e, ang, ax, eo);
}
// -------------------------------------------------
// 双倍点数
void doublevnum(VECLIST& e, int num = 0)
{
	VECLIST te;
	int n = num == 0 ? e.size() - 1 : num;
	
	for(int i = 0; i < n; i ++)
	{		
		te.PUSH(e[i]);
		te.PUSH(e[i]);
		te.back().p = (e[i].p + e[(i+1) % n].p) / 2;
	}
	te.PUSH(e[e.size() - 1]);
	e = te;
}
// -------------------------------------------------
// 双倍点数
void doublevnum(VECLIST& e, VECLIST& oe, int num = 0)
{
    int n = num == 0 ? e.size() - 1 : num;
    
    for(int i = 0; i < n; i ++)
    {
        oe.PUSH(e[i]);
        oe.PUSH(e[i]);
		oe.back().p = (e[i].p + e[(i + 1) % n].p) / 2;
    }
	oe.PUSH(e[e.size() - 1]);
}
// 闭合
void closeedge(VECLIST& e)
{
	if(e.empty())
		return;
	
	e.PUSH(e[0]);
}
// 链接
void linkedge(VECLIST& e, const VECLIST& le, bool inv = false)
{
	if(inv == false)
	{
		for(int i = 0; i < le.size(); i ++)
		{
			e.PUSH(le[i]);
		}
	}
	else
	{
		for(int i = le.size() - 1; i >= 0; i --)
		{
			e.PUSH(le[i]);
		}
	}
}
// 添加中点
void insertp(VECLIST& e, crvec p, int pos)
{
	VECLIST te;
	for(int i = 0; i < e.size(); i ++)
	{		
		te.PUSH(e[i]);
		if(i == pos)	
			te.PUSH(p);			
	}
	e = te;
}
int insertp(VECLIST& e, crvec p, crvec pre, crvec aft)
{
	int pos = -1;
	VECLIST te;
	int i = 0;
	for(; i < e.size() - 1; i ++)
	{		
		te.PUSH(e[i]);
		if(e[i] == pre && e[i + 1] == aft)
		{		
			pos = i;
			te.PUSH(p);		
		}			
	}
	te.PUSH(e.back());
	e = te;
	return i;
}
void blendedge(const VECLIST& e1, const VECLIST& e2, real alpha, VECLIST& eo)
{
	if(e1.size() != e2.size())
		return;
	for(int i = 0; i < e1.size(); i ++)
	{
		eo.PUSH(blend(e1[i].p, e2[i].p, alpha));
	}	
}
// -------------------------------------------------
// 圆边沿
void roundedge(VECLIST& e, crvec o, crvec vx, crvec vy, real r, int sig, real ang0)
{
	if (bclosed && gcommonvertex)
	{
		CIRCLE2(i, r, sig, ang0)
			e.PUSH(o + (vx * dpi.x + vy * dpi.y));
		END
	}
	else
	{
		CIRCLE3(i, r, sig, ang0)
			e.PUSH(o + (vx * dpi.x + vy * dpi.y));
		END
	}
}
inline void roundedgex(VECLIST& e, crvec o, crvec vx, crvec vy, real r, int sig, real ang, real ang0)
{
	CIRCLE(i, r, sig, ang, ang0)
		e.PUSH(o + (vx * dpi.x + vy * dpi.y));
	END
}
void roundedge(VECLIST& e, crvec o, vec v, real r, int sig, real ang0 = 0)
{
	v.norm();
	vec dx, dy;
	v2vxvy(v, dx, dy);
	roundedge(e, o, dx, dy, r, sig, ang0);
}
// -------------------------------------------------
// match point
int matchpoint(const VECLIST& e, crvec p, real mindis = 0.000001)
{
	for(int i = 0; i < e.size(); i ++)
	{		
		if((p - e[i].p).sqrlen() < mindis)
		{			
			return i;
		}			
	}
	return -1;
}
// -------------------------------------------------
bool matchedge(const VECLIST& re, VECLIST& e)
{
	real dis = (getedgecenter(re) - getedgecenter(e)).len();
	vec v = getedgenorm(re) * dis;
	VECLIST ne;
	for(int i = 0; i < 4; i ++)
	{
		int ind = matchpoint(e, re[i].p + v, 0.00001);
		if(-1 == ind)
			return false;
		ne.PUSH(e[ind]);		
	}
	e = ne;
	return true;
}
// -------------------------------------------------
// 一些常用扩展
// -------------------------------------------------
void extrudeedgeex(VECLIST& e, real d, real s = 1)
{
	VECLIST e1;		
	extrudeedge(e, d, e1);
	if (s != 1)
		scaleedge(e1, s);
	face(e, e1);
	e = e1;
}
void extrudeedgeex1(VECLIST& e, real d, real s = 1)
{
	VECLIST e1;
	extrudeedge(e, d, e1);
	if (s != 1)
		scaleedge(e1, s);
	e = e1;
}
void extrudeedgeex2(VECLIST& e, real d, real r)
{
    VECLIST e1;
    extrudeedge(e, d, e1);
    radedge(e1, r);
    face(e, e1);
    //	face(e1);
    e = e1;
}
void extrudeedgeex3(VECLIST& e, real d, real r)
{
	radedge2(e, r);
	extrudeedge(e, d);
}
void extrudeedgeex4(VECLIST& e, real d, real r)
{
	radedge2(e, r);
	VECLIST e1;
	extrudeedge(e, d, e1);
	face(e, e1);
	e = e1;
}
// -------------------------------------------------
void extrudeedgeex(const VECLIST& e, real d, real s, VECLIST& oe)
{	
	extrudeedge(e, d, oe);
	scaleedge(oe, s);
}
void scaleedgeex(VECLIST& e, real s)
{
	VECLIST e1;
	scaleedge(e, s, e1);
	face(e, e1);
	e = e1;
}
// ================================================================================================
// 小件编程
// ================================================================================================

// -------------------------------------------------
// 管道细节
// -------------------------------------------------
void addpip(VECLIST& e, real l)
{	
	extrudeedgeex(e, l, 1);	
}
void addpipS(VECLIST& e, real s)
{	
	VECLIST ee;
	scaleedge(e, s, ee);	
	face(e, ee);
	e = ee;
}
void addpipL(VECLIST& e, real l, crvec ax)
{		
	// 转折
	VECLIST ee;
	rotedge(e, PI / 2, ax, ee);	
	face(e, ee);	
	
	e = ee;	
}
void addpipR(VECLIST& e, real l, crvec ax)
{	
	// 转折
	VECLIST ee;
	rotedge(e, -PI / 2, ax, ee);	
	face(e, ee);	
	
	e = ee;	
}

// ------------------------------------------------
// 方片结构
// ------------------------------------------------
void plane(VECLIST& be, crvec o, vec up, real sx, real sy)
{
	up.norm();
	vec vx, vy;
	v2vxvy(up, vx, vy);
	
	vec p1 = o - vx * (sx / 2) + vy * (sy / 2);
	vec p2 = o - vx * (sx / 2) - vy * (sy / 2);
	vec p3 = o + vx * (sx / 2) - vy * (sy / 2);
	vec p4 = o + vx * (sx / 2) + vy * (sy / 2);
	be.PUSH(p1);be.PUSH(p2);be.PUSH(p3);be.PUSH(p4);
}
void plane(VECLIST& be, crvec o, vec vx, vec vy, real sx, real sy)
{
	vec p1 = o - vx * (sx / 2) + vy * (sy / 2);
	vec p2 = o - vx * (sx / 2) - vy * (sy / 2);
	vec p3 = o + vx * (sx / 2) - vy * (sy / 2);
	vec p4 = o + vx * (sx / 2) + vy * (sy / 2);
	be.PUSH(p1); be.PUSH(p2); be.PUSH(p3); be.PUSH(p4);
}
// ------------------------------------------------
// 圆形结构
// ------------------------------------------------
void round(VECLIST& be, crvec o, vec up, real s, int num = 8, real ang0 = 0)
{
	up.norm();
	vec vx, vy;
	v2vxvy(up, vx, vy);	
	roundedge(be, o, vx, vy, s, num, ang0);	
}
void roundex(VECLIST& be, crvec o, vec up, real s, int num = 8, real ang0 = 0, real sx = 1, real sy = 1)
{
	up.norm();
	vec vx, vy;
	v2vxvy(up, vx, vy);
	roundedge(be, o, vx * sx, vy * sy, s, num, ang0);
}
inline bool  checkPointInTriangle2(
	const vec3& point,
	const vec3& a, const vec3& b, const vec3& c,
	float tolerance = 0.005f)
{
	real total_angles = 0.0f;

	// make the 3 vectors

	vec3 v1(point.x - a.x, point.y - a.y);
	v1.norm();
	vec3 v2(point.x - b.x, point.y - b.y);
	v2.norm();
	vec3 v3(point.x - c.x, point.y - c.y);
	v3.norm();

	real Dot1 = v2.dot(v1);
	Dot1 < -1.0f ? Dot1 = -1.0f : NULL;
	Dot1 > 1.0f ? Dot1 = 1.0f : NULL;
	total_angles += acos(Dot1);

	real Dot2 = v3.dot(v2);
	Dot2 < -1.0f ? Dot2 = -1.0f : NULL;
	Dot2 > 1.0f ? Dot2 = 1.0f : NULL;
	total_angles += acos(Dot2);

	real Dot3 = v1.dot(v3);
	Dot3 < -1.0f ? Dot3 = -1.0f : NULL;
	Dot3 > 1.0f ? Dot3 = 1.0f : NULL;
	total_angles += acos(Dot3);

	if (fabs(total_angles - 2.0f * PI) <= tolerance)
		return true;

	return false;
}
bool checkintir(VECLIST& rpoly, std::vector<int>& insidelist, int ind1, int ind2, int ind3)
{
	insidelist.clear();
	{// 检查没有点在三角形内			
		vec3 p0 = rpoly[ind1];
		vec3 p1 = rpoly[ind2];
		vec3 p2 = rpoly[ind3];
		for (int k = 0; k < rpoly.size(); k++)
		{
			if (k != ind1 && k != ind2 && k != ind3 &&
				checkPointInTriangle2(rpoly[k], p0, p1, p2))
			{
				insidelist.push_back(k);
			}
		}
	}
	return insidelist.size() > 0;
}

void linktri(VECLIST& rpoly, std::vector<vec3>& trianglelist, int depth = 0)
{
	if (rpoly.size() == 3)
	{// 最后一个三角形
		vec3 p0 = rpoly[0];
		vec3 p1 = rpoly[1];
		vec3 p2 = rpoly[2];

		//	if ((p1 - p0).cross(p2 - p0) < 0)
		{
			trianglelist.push_back(p0);
			trianglelist.push_back(p1);
			trianglelist.push_back(p2);
		}
		return;
	}
	if (rpoly.size() < 3 || depth >= 500)
		return;
	{
		// find left most triangle
		int findk = -1;
		for (int k = 0; k < rpoly.size(); k++)
		{
			if (findk == -1 || rpoly[k].p.x < rpoly[findk].p.x)
			{
				findk = k;
			}
		}
		int ind1 = findk - 1;
		int ind2 = findk;
		int ind3 = findk + 1;
		if (findk == 0)
			ind1 = rpoly.size() - 1;
		else if (findk == rpoly.size() - 1)
			ind3 = 0;

		{// inside the triangle
			int indm = ind3;
			std::vector<int> insidelist;
			if (checkintir(rpoly, insidelist, ind1, ind2, indm))
			{
				// left most inside
				int findk2 = -1;
				for (int i = 0; i < insidelist.size(); i++)
				{
					int k = insidelist[i];
					if (findk2 == -1 || rpoly[k].p.x < rpoly[findk2].p.x)
					{
						findk2 = k;
					}
				}
				indm = findk2;
			}
			if (indm != ind3)
			{
				{// 添加当前的三角形
					vec3 p0 = rpoly[ind1];
					vec3 p1 = rpoly[ind2];
					vec3 p2 = rpoly[indm];

					//if ((p1 - p0).cross(p2 - p0) < 0)
					{
						trianglelist.push_back(p0);
						trianglelist.push_back(p1);
						trianglelist.push_back(p2);
					}
				}
				{
					VECLIST poly1;
					for (int i = ind2; i != indm; i = (i + 1) % rpoly.size())
					{
						poly1.push_back(rpoly[i]);
					}
					poly1.push_back(rpoly[indm]);
					linktri(poly1, trianglelist, depth + 1);
				}
				{
					VECLIST poly2;
					for (int i = indm; i != ind1; i = (i + 1) % rpoly.size())
					{
						poly2.push_back(rpoly[i]);
					}
					poly2.push_back(rpoly[ind1]);

					linktri(poly2, trianglelist, depth + 1);
				}
				return;
			}
		}
		{
			vec3 p0 = rpoly[ind1];
			vec3 p1 = rpoly[ind2];
			vec3 p2 = rpoly[ind3];

			{
				trianglelist.push_back(p0);
				trianglelist.push_back(p1);
				trianglelist.push_back(p2);
			}
			rpoly.erase(rpoly.begin() + findk);

		}
		linktri(rpoly, trianglelist, depth + 1);
	}
}

void GenPrism(VECLIST& e, vec3 up)
{
	VECLIST ee = e;
	moveedge(ee, up);
	face(e, ee);

	{// up
		std::vector<vec3> trianglelist;
		linktri(e, trianglelist);
		for (int i = 0; i < trianglelist.size(); i += 3)
		{
			vertex v1 = trianglelist[i];
			v1.n = vec3::UY;
			vertex v2 = trianglelist[i + 1];
			v2.n = vec3::UY;
			vertex v3 = trianglelist[i + 2];
			v3.n = vec3::UY;
			triang(v1, v2, v3);
		}
	}
	{// down
		std::vector<vec3> trianglelist;
		linktri(ee, trianglelist);
		
		for (int i = 0; i < trianglelist.size(); i += 3)
		{
			vertex v1 = trianglelist[i];
			v1.n = -vec3::UY;
			vertex v2 = trianglelist[i + 1];
			v2.n = -vec3::UY;
			vertex v3 = trianglelist[i + 2];
			v3.n = -vec3::UY;
			triang(v1, v2, v3);
		}
	}
}