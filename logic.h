// *****************************************************************
int gamelevel = 1;
int gamestate = 1; // 1 playable 2 movie
int gamescene = 1;
int gamestep = 1;

int camstate = 3;
int galpha = 0;

vec mousepos;
// ****************************************************************
// 逻辑
// ****************************************************************
real disx(const vec& p1, const vec& p2)
{
	return fabs(p1.x - p2.x);
}
real disy(const vec& p1, const vec& p2)
{
	return fabs(p1.y - p2.y);
}
// ================================================================
// 更新
// ----------------------------------------------------------------
void update()
{
    
    //////////////
    framecnt ++;
	gtime += frametime;
}
// ----------------------------------------------------------------
// 点击操作
// ----------------------------------------------------------------
void removeitem(int ind)
{
	LOGMSG("removeitem " << ind);
}

// ================================================================
extern void onpick(real x, real y);
extern "C"
{
	int lstid = 0;
	int lstfrmcnt = 0;
	void EXPORT_API onbutton(int id, real dirx, real diry)
	{
		PRINT("onbutton " << id)
	}
    void EXPORT_API onevent(int id, int key, real x, real y)
    {
		if (id == 0 && key == 0)
		{
			onpick(x, y);
			PRINT("onpick")
		}
    }
	void EXPORT_API onpickobj(int id, real hitpos[3], int triind)
	{
		PRINT("onpickobj " << id)
	}
    void EXPORT_API updatecolor(float pos[3], unsigned int cor)
    {
        galpha = cor;
    }
    int EXPORT_API getgamelevel(){
        return gamelevel;
    }
    int EXPORT_API getgamestate(){
        return gamestate;
    }
    int EXPORT_API getcamstate(){
        return camstate;
    }
    void EXPORT_API setcampos(real x, real y, real z){
        campos = vec(x, y, z);
    }
    bool EXPORT_API getcampos(float pos[3])
    {
        pos[0] = campos.x;
        pos[1] = campos.y;
		return false;
    }
	int EXPORT_API getintdat(int ind)
	{
		if (ind == 2)
		{
			return gamescene;
		}
		return 0;
	}
	void EXPORT_API setintdat(int ind, int val)
	{
		if (ind == 2)
		{
			gamescene = val;
		}
	}
	int EXPORT_API windowstate(int id)
	{
		return 0;
	}
	void EXPORT_API setframetime(real frmtime)
	{
		frametime = frmtime;
	}
}