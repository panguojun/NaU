void mountain(real ang0, real scale)
{
    DEF_SCALES(8);
    const int len = 8;
    const int wid = 8;
    vec lstpt[wid + 1];
    vec llstpt[wid + 1];
    vec lstp;
    real xuelevel = 1.2;
    real wetlevel = 0.15;
    binvnorm = 1;
    gcommonvertex = 1;
    int steps = 0;
    for (int i = 0; i < len; i++)
    {
        real r = 0.1 + (i / real(len));
        for (int j = 0; j < wid; j++)
        {
            real ang = ang0 + (j / real(wid - 1)) * PI * 2;// / 4;
            //vec2 p(i / real(len) - 0.5, j / real(wid - 1) - 0.5);
            vec2 p = vec2(r * cos(ang), r * sin(ang));
            real x = p.x * scale;
            real y = 3.85 + (p.x * p.y) / 11.8;
            real z = p.y * scale;
            vec pp = vec(x, 0, z);
            {
                real g0 = RidgedMF(x * .085, y * .095, z * .0715, 6, 1); // 山
                real deta1 = s3 * blend(-0.25, 10, g0, 4); // 山脉
                real deta2 = s4 * fBm(x * 2.75, y * 1.51, z * 1.25, 8); // 地表
                real deta = MAX(deta1, deta2);
                pp += vec::UY * ((deta)*blend(1.0, 0, r, 1 + 2 * deta1));

                if (i > 0 && j > 0)
                {
                    lummode = -1;
                    if (deta > xuelevel)
                        color = RGB(255, 255, 255);
                    else
                    {
                        color = blendcor(RGB(150, 160, 150), RGB(50, 50, 50), deta / xuelevel, 0.5 + deta2 * 1);

                    }
                    plane0(pp, lstp, llstpt[j], lstpt[j]);
                    lummode = 0;
                }
            }
            if (j == 0 || j == wid)
                llstpt[j] = pp;
            else
                llstpt[j] = lstp;
            lstpt[j] = lstp = pp;
        }
    }
    binvnorm = 0;
    gcommonvertex = 0;
}
real polygonisecell(int i, int j, int k)
{
    const int SIZE = luaparam[0];
    vec o(SIZE / 2.0f, SIZE / 2.0f, SIZE / 2.0f);
    o *= luaparam[4];

    GRIDCELL grid;
    grid.p[0] = vec(i, j, k) * luaparam[4];
    grid.p[1] = vec(i + 1, j, k) * luaparam[4];
    grid.p[2] = vec(i + 1, j, k + 1) * luaparam[4];
    grid.p[3] = vec(i, j, k + 1) * luaparam[4];
    grid.p[4] = vec(i, j + 1, k) * luaparam[4];
    grid.p[5] = vec(i + 1, j + 1, k) * luaparam[4];
    grid.p[6] = vec(i + 1, j + 1, k + 1) * luaparam[4];
    grid.p[7] = vec(i, j + 1, k + 1) * luaparam[4];

    grid.val[0] = (grid.p[0] - o).len() + luaparam[2] * _MIN(0, fBm(grid.p[0].x / luaparam[3], grid.p[0].y / luaparam[3], grid.p[0].z / luaparam[3], 8) - luaparam[5]);
    grid.val[1] = (grid.p[1] - o).len() + luaparam[2] * _MIN(0, fBm(grid.p[1].x / luaparam[3], grid.p[1].y / luaparam[3], grid.p[1].z / luaparam[3], 8) - luaparam[5]);
    grid.val[2] = (grid.p[2] - o).len() + luaparam[2] * _MIN(0, fBm(grid.p[2].x / luaparam[3], grid.p[2].y / luaparam[3], grid.p[2].z / luaparam[3], 8) - luaparam[5]);
    grid.val[3] = (grid.p[3] - o).len() + luaparam[2] * _MIN(0, fBm(grid.p[3].x / luaparam[3], grid.p[3].y / luaparam[3], grid.p[3].z / luaparam[3], 8) - luaparam[5]);
    grid.val[4] = (grid.p[4] - o).len() + luaparam[2] * _MIN(0, fBm(grid.p[4].x / luaparam[3], grid.p[4].y / luaparam[3], grid.p[4].z / luaparam[3], 8) - luaparam[5]);
    grid.val[5] = (grid.p[5] - o).len() + luaparam[2] * _MIN(0, fBm(grid.p[5].x / luaparam[3], grid.p[5].y / luaparam[3], grid.p[5].z / luaparam[3], 8) - luaparam[5]);
    grid.val[6] = (grid.p[6] - o).len() + luaparam[2] * _MIN(0, fBm(grid.p[6].x / luaparam[3], grid.p[6].y / luaparam[3], grid.p[6].z / luaparam[3], 8) - luaparam[5]);
    grid.val[7] = (grid.p[7] - o).len() + luaparam[2] * _MIN(0, fBm(grid.p[7].x / luaparam[3], grid.p[7].y / luaparam[3], grid.p[7].z / luaparam[3], 8) - luaparam[5]);


    TRIANGLE tri[5];
    int num = Polygonise(grid, luaparam[1], tri);
    num = MIN(num, 5);

    for (int i = 0; i < num; i++)
    {
        vertex v1, v2, v3;
        v1.p = (tri[i].p[0] - o) * 5;
        v2.p = (tri[i].p[1] - o) * 5;
        v3.p = (tri[i].p[2] - o) * 5;

        v1.n = v1.p.normcopy();
        v2.n = v2.p.normcopy();
        v3.n = v3.p.normcopy();
        triang(v1, v2, v3);
    }
    return grid.val[0];
}

// -------------------------------------------------
//byte volum[512][512][512] = { 0 };
//void buildrock2(int x, int y, int z, int depth)
//{
//    volum[x][y][z] = depth + 1;
//
//    if (depth > 2000)
//        return;
//    int nx = x, ny = y, nz = z;
//    int rv = rand() % 6;
//    if (rv == 0)
//        nx = x + 1;
//    else if (rv == 1)
//        nx = x - 1;
//    else if (rv == 2)
//        ny = y + 1;
//    else if (rv == 3)
//        ny = y - 1;
//    else if (rv == 4)
//        nz = z + 1;
//    else if (rv == 5)
//        nz = z - 1;
//
//    if (nx >= 0 && nx < 512 &&
//        ny >= 0 && ny < 512 &&
//        nz >= 0 && nz < 512 &&
//        volum[nx][ny][nz] == 0
//        )
//        buildrock2(nx, ny, nz, depth + 1);
//}
//
//void buildrock(int x, int y, int z, int depth)
//{
//    volum[x][y][z] = depth;
//
//    if (depth > 10)
//        return;
//    int nx = x, ny = y, nz = z;
//    for (int i = 0; i < 6; i++)
//    {
//        int rv = rand() % 6;
//        if (rand() % 6 == 0)
//            nx = x + 1;
//        else if (rand() % 6 == 1)
//            nx = x - 1;
//        else if (rand() % 6 == 2)
//            ny = y + 1;
//        else if (rand() % 6 == 3)
//            ny = y - 1;
//        else if (rand() % 6 == 4)
//            nz = z + 1;
//        else
//            nz = z - 1;
//
//        if (nx >= 0 && nx < 512 &&
//            ny >= 0 && ny < 512 &&
//            nz >= 0 && nz < 512 &&
//            volum[nx][ny][nz] == 0
//            )
//            buildrock(nx, ny, nz, depth + 1);
//    }
//}

// -------------------------------------------------------------------------
// 遍历相邻三角形算法
// -------------------------------------------------------------------------
struct tricalc_t
{
    int tri1, tri2;
};
int findneighbor(submesh& sm, crvec pp2, crvec pp3)
{
    for (int i = 0; i < sm.tris.size(); i++)
    {
        int t1 = sm.tris[i].vertexIndex[0];
        int t2 = sm.tris[i].vertexIndex[1];
        int t3 = sm.tris[i].vertexIndex[2];

        vertex& p1 = sm.vertices[t1];
        vertex& p2 = sm.vertices[t2];
        vertex& p3 = sm.vertices[t3];

        if (sm.tris[i].bread)
        {
            continue;
        }
      
        if (pp2 == p1 && pp3 == p3 ||
            pp2 == p2 && pp3 == p1 || 
            pp2 == p3 && pp3 == p2 
            )
        {
            PRINT("findneighbor " << i);
            //sm.tris[i].bread = true;
            return i;
        }
    }
    return -1;
}
vec calcnorm(const submesh& sm, int index)
{
    int t1 = sm.tris[index].vertexIndex[0];
    int t2 = sm.tris[index].vertexIndex[1];
    int t3 = sm.tris[index].vertexIndex[2];
    const vertex& p1 = sm.vertices[t1];
    const vertex& p2 = sm.vertices[t2];
    const vertex& p3 = sm.vertices[t3];

    vec nm = (p2.p - p1.p).cross(p3.p - p1.p);
    nm.norm();
    return nm;
}

std::vector< tricalc_t> greault;
void calc(submesh& sm, int index, int depth = 0)
{   
    sm.tris[index].bread = true;

    int t1 = sm.tris[index].vertexIndex[0];
    int t2 = sm.tris[index].vertexIndex[1];
    int t3 = sm.tris[index].vertexIndex[2];

    vertex& p1 = sm.vertices[t1];
    vertex& p2 = sm.vertices[t2];
    vertex& p3 = sm.vertices[t3];

    color = 0xFFFF0000;
    triang0(p1, p2, p3);

    vec nm = (p2.p - p1.p).cross(p3.p - p1.p);
    nm.norm();

    for(int i = 0; i < 3; i ++)
    {            
        int nb = -1;
        if(i == 0)
            nb = findneighbor(sm, p2, p3);
        else if (i == 1)
            nb = findneighbor(sm, p3, p1);
        else
            nb = findneighbor(sm, p1, p2);
        if (-1 == nb)
        {
            continue;
        }
        tricalc_t tc;
       

        // 检查法线是否为并集
        vec nm2 = calcnorm(sm, nb);
        if (nm.cross(nm2).dot(p3 - p2) > 0)
        {
            continue;
        }
        
        tc.tri1 = index;
        tc.tri2 = nb;
        greault.push_back(tc);
        PRINT("tc: (" << tc.tri1 << "," << tc.tri2 << ")");

        // 递归
        if(depth < 1000)
            calc(sm, nb, depth + 1);
    }
}
// -------------------------------------------------------------------------
// 内外关系体素分隔法
// -------------------------------------------------------------------------
const int DEPTH = 250;

real UNITSIZE = 0.25;
const int WIDTH = 100;
const int HEIGHT = 50;
const int LENGTH = 100;

struct vec3i_t{int x = 0, y = 0, z = 0;};
vec3i_t CENTER, SCENEMIN, SCENEMAX;

bool volum[WIDTH][HEIGHT][LENGTH] = { 0 };
byte volumid[WIDTH][HEIGHT][LENGTH] = { 0 };

std::vector< vec3i_t> gworkpoints;

void buildaabb(submesh& sm, boundingbox& aabb)
{
    vec3 aa(10000, 10000, 10000);
    vec3 bb(-10000, -10000, -10000);
    for (int i = 0; i < sm.tris.size(); i++)
    {
        int t1 = sm.tris[i].vertexIndex[0];
        int t2 = sm.tris[i].vertexIndex[1];
        int t3 = sm.tris[i].vertexIndex[2];

        vertex* pts[3];
        pts[0] = &sm.vertices[t1];
        pts[1] = &sm.vertices[t2];
        pts[2] = &sm.vertices[t3];

        for (int j = 0; j < 3; j ++)
        {
            if (pts[j]->p.x < aa.x)
                aa.x = pts[j]->p.x;
            if (pts[j]->p.y < aa.y)
                aa.y = pts[j]->p.y;
            if (pts[j]->p.z < aa.z)
                aa.z = pts[j]->p.z;

            if (pts[j]->p.x > bb.x)
                bb.x = pts[j]->p.x;
            if (pts[j]->p.y > bb.y)
                bb.y = pts[j]->p.y;
            if (pts[j]->p.z > bb.z)
                bb.z = pts[j]->p.z;
        }
    }
    aabb.a = aa;
    aabb.b = bb;

    PRINT("sm.tris.size()=" << sm.tris.size());
    PRINTVEC("aa", aa);
    PRINTVEC("bb", bb);
}
struct model_t
{
    boundingbox aabb;
    submesh* psm = 0;  

    model_t(submesh* _psm)
    {
        psm = _psm;
        buildaabb(*psm, aabb);
    }
};
bool hittest(const submesh& sm, crvec rp1, crvec rp2, bool bout2in)
{
    vec v12 = rp2 - rp1;
    real d12 = v12.len();
    v12.norm();

    for (int i = 0; i < sm.tris.size(); i++)
    {
        int t1, t2, t3;
        if (bout2in)
        {
            t1 = sm.tris[i].vertexIndex[0];
            t2 = sm.tris[i].vertexIndex[1];
            t3 = sm.tris[i].vertexIndex[2];
        }
        else
        {
            t1 = sm.tris[i].vertexIndex[0];
            t3 = sm.tris[i].vertexIndex[1];
            t2 = sm.tris[i].vertexIndex[2];
        }

        const vertex& p1 = sm.vertices[t1];
        const vertex& p2 = sm.vertices[t2];
        const vertex& p3 = sm.vertices[t3];

        {// 终点三角形顶点重合
            if ((rp2 - p1.p).sqrlen() < UNITSIZE * UNITSIZE ||
                (rp2 - p2.p).sqrlen() < UNITSIZE * UNITSIZE ||
                (rp2 - p3.p).sqrlen() < UNITSIZE * UNITSIZE)
            {
                point(rp2, 10, 0xFFff00ff);
                PRINT("终点三角形顶点重合");
                return true;
            }
        }   
     
        vec nm = (p2.p - p1.p).cross(p3.p - p1.p).normcopy();
        vec hp;
        planeAndSLIntersectionV(nm, nm.dot(p1.p), rp1, v12, hp);
        real deta = (hp - rp1).dot(v12);

        if(deta < -UNITSIZE || deta > d12 + UNITSIZE)
        {// 不在线段上
            continue;
        }
        {// 碰撞点与三角形顶点重合
            if ((hp - p1.p).sqrlen() < UNITSIZE * UNITSIZE ||
                (hp - p2.p).sqrlen() < UNITSIZE * UNITSIZE ||
                (hp - p3.p).sqrlen() < UNITSIZE * UNITSIZE)
            {
                point(hp, 10, 0xFFff00ff);
                PRINT("碰撞点与三角形顶点重合");
                return true;
            }
        }
        // 三角形内            
        if (checkPointInTriangle(hp, p1.p, p2.p, p3.p, UNITSIZE))
        {
            point(hp, 8, 0xFFff00a0);
            PRINT("三角形内");
            return true;
        }        
    }
    return false;
}
bool isin(model_t& md, crvec pt)
{
    bool bin = md.aabb.intersection(pt);

    return bin;
}

inline vec ivec(int x, int y, int z)
{
    return(vec((real)x - CENTER.x, (real)y - CENTER.y, (real)z - CENTER.z)) * UNITSIZE;
}

void inout(model_t& md, int x, int y, int z, bool bin, int depth)
{
    if (depth > DEPTH)
    {
        // PRINT("depth > " << depth);
        vec3i_t wp;
        wp.x = x;
        wp.y = y;
        wp.z = z;
        gworkpoints.push_back(wp);
        return;
    }

    if (volum[x][y][z] != 0)
    {
        PRINT("重复计算: " << x << "," << y << "," << z);
        return;
    }
    volum[x][y][z] = 1;

    //static int count = 0;
   // PRINT("walk " << "depth=" <<depth << " count=" << (count++) << ":" << x << "," << y << "," << z);
    point(ivec(x, y, z), 8, bin == 1 ? 0xFF00FF00 : 0xFF0000FF);

    //if (depth > DEPTH)
    //{
    //   // PRINT("depth > " << depth);
    //    
    //    return;
    //}   
    for (int i = 0; i < 6; i ++)
    {
        int nx = x, ny = y, nz = z;        
        {// ALL DIRECTIONS
            if (i == 0)
                nx = x + 1;
            else if (i == 1)
                nx = x - 1;
            else if (i == 2)
                ny = y + 1;
            else if (i == 3)
                ny = y - 1;
            else if (i == 4)
                nz = z + 1;
            else
                nz = z - 1;
        }
        if (nx >= SCENEMIN.x && nx < SCENEMAX.x &&
            ny >= SCENEMIN.y && ny < SCENEMAX.y &&
            nz >= SCENEMIN.z && nz < SCENEMAX.z &&
            volum[nx][ny][nz] == 0
            )
        {       
            {// 方格之间射线检测是否可连接
                if (bin)
                {// in
                    if (isin(md, ivec(nx, ny, nz)) && !hittest(*md.psm, ivec(x, y, z), ivec(nx, ny, nz), false))                  
                    {                        
                       // PRINT("in " << int(val) << ":" << nx << "," << ny << "," << nz);
                        inout(md, nx, ny, nz, 1, depth + 1);
                    }
                }
                else
                {// out
                    if (!(isin(md, ivec(nx, ny, nz)) && hittest(*md.psm, ivec(x, y, z), ivec(nx, ny, nz), true)))                   
                    {
                        //PRINT("out " << int(val) << ":" << nx << "," << ny << "," << nz);
                        inout(md, nx, ny, nz, 0, depth + 1);
                    }
                }
            } 
        }
    }
}
void calc(model_t& md, byte val)
{
    CENTER.x = WIDTH / 2;
    CENTER.y = HEIGHT / 2;
    CENTER.z = LENGTH / 2;

    // 根据AABB计算范围

    SCENEMIN.x = md.aabb.a.x / UNITSIZE + CENTER.x - 1;
    SCENEMIN.y = md.aabb.a.y / UNITSIZE + CENTER.y - 1;
    SCENEMIN.z = md.aabb.a.z / UNITSIZE + CENTER.z - 1;

    SCENEMIN.x < 0 ? SCENEMIN.x = 0 : 0;
    SCENEMIN.y < 0 ? SCENEMIN.y = 0 : 0;
    SCENEMIN.z < 0 ? SCENEMIN.z = 0 : 0;

    SCENEMAX.x = md.aabb.b.x / UNITSIZE + CENTER.x + 1;
    SCENEMAX.y = md.aabb.b.y / UNITSIZE + CENTER.y + 1;
    SCENEMAX.z = md.aabb.b.z / UNITSIZE + CENTER.z + 1;

    SCENEMAX.x > WIDTH  ? SCENEMAX.x = WIDTH  : 0;
    SCENEMAX.y > HEIGHT ? SCENEMAX.y = HEIGHT : 0;
    SCENEMAX.z > LENGTH ? SCENEMAX.z = LENGTH : 0;

    // 在外边计算（AABB范围），最后合成  

    gworkpoints.clear();
    //inout(md, SCENEMIN.x, SCENEMIN.y, SCENEMIN.z, 0, 0);
    inout(md, SCENEMIN.x, SCENEMIN.y, (SCENEMIN.z + SCENEMAX.z) / 2, 0, 0);

    for (int i = 0; i < gworkpoints.size(); i++)
    {
        vec3i_t wp = gworkpoints[i];

        if (volum[wp.x][wp.y][wp.z] == 0)
            inout(md, wp.x, wp.y, wp.z, 0, 0);
    }
}
// -------------------------------------------------------------------------
// 测试与外部调用
// -------------------------------------------------------------------------

void starterrain(int cutindex, int cutsize)
{
    const int SIZE = luaparam[0];
    vec o(SIZE / 2.0f, SIZE / 2.0f, SIZE / 2.0f);
    o *= luaparam[4];
    for (int i = 0; i < SIZE; i++)
    {
       // if (cutindex == int(i / cutsize))
        for (int i = 0; i < SIZE; i++)
        {
            for (int j = 0; j < SIZE; j++)
            {
                for (int k = 0; k < SIZE; k++)
                {
                    polygonisecell(i, j, k);
                }
            }
        }
    }
    
    ZeroMemory(volumid, sizeof(volumid));

    model_t md(&gsubmesh);
    
    for (int i = 0; i < 1; i++)
    {
        byte val = i + 1;

        ZeroMemory(volum, sizeof(volum));

        calc(md, val);

        for (int i = SCENEMIN.x; i < SCENEMAX.x; i++)
        {
            for (int j = SCENEMIN.y; j < SCENEMAX.y; j++)
            {
                for (int k = SCENEMIN.z; k < SCENEMAX.z; k++)
                {
                    if (volum[i][j][k] == 0)
                    {
                        volumid[i][j][k] = val;
                        if(k == LENGTH / 2)
                            pixel(ivec(i, j, k), 0xFFFFFF00);
                    }
                    else
                    {
                        pixel(ivec(i, j, k), 0xFF00FFFF);
                    }
                }
            }
        }
    }
}
// -------------------------------------------------------------------------
// 凹面体分割
// -------------------------------------------------------------------------
void cuttriang(submesh& sm, triangle tri, crvec p, crvec nm)
{
    int t1 = tri.vertexIndex[0];
    int t2 = tri.vertexIndex[1];
    int t3 = tri.vertexIndex[2];

    const vertex& p1 = sm.vertices[t1];
    const vertex& p2 = sm.vertices[t2];
    const vertex& p3 = sm.vertices[t3];


    vec v1 = p1 - p;
    real d1 = v1.dot(nm);
    vec v2 = p2 - p;
    real d2 = v2.dot(nm);
    vec v3 = p3 - p;
    real d3 = v3.dot(nm);

    if (d1 * d2 < 0 && d1 * d3 > 0)
    {
        vec p12;
        planeAndSLIntersectionV(nm, nm.dot(p), p1.p, p2.p-p1.p, p12);
        vec p23;
        planeAndSLIntersectionV(nm, nm.dot(p), p2.p, p3.p - p2.p, p23);
        triang0(p1, p12, p3);
        triang0(p12, p23, p3);
        triang0(p2, p23, p12);
        return;
    }
    else if (d1 * d2 > 0 && d1 * d3 < 0)
    {
        vec p13;
        planeAndSLIntersectionV(nm, nm.dot(p), p1.p, p3.p - p1.p, p13);
        vec p23;
        planeAndSLIntersectionV(nm, nm.dot(p), p2.p, p3.p - p2.p, p23);
        triang0(p1, p2, p13);
        triang0(p13, p2, p23);
        triang0(p23, p3, p13);
        return;
    }
    else if (d1 * d2 < 0 && d1 * d3 < 0)
    {
        vec p12;
        planeAndSLIntersectionV(nm, nm.dot(p), p1.p, p2.p - p1.p, p12);
        vec p13;
        planeAndSLIntersectionV(nm, nm.dot(p), p1.p, p3.p - p1.p, p13);
        triang0(p1, p12, p13);
        triang0(p12, p2, p13);
        triang0(p13, p2, p3);
        return;
    }
}
