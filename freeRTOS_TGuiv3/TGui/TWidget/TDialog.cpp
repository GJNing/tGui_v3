#include "TDialog.h"



TDialog::TDialog(int32 x, int32 y, int32 w, int32 h, const char* n, TWidget* obj)
	:TWidget(x, y, w, h, n, obj)
{
	contactOffsetW = -1;
	contactOffsetH = -1;
	preX = -1;
	preY = -1;
	state = false;
	hasFocus = true;
	setBackColor(MIDLIGHT);
	//setIsVariable(true);//设置为可变状态 --> 加上边界无效区
	TBufPainter p(getBuffer()->getBufAddr(),getRect());
	p.drawDialog(0, 0, width(), height(), getName(), hasFocus, getBackColor());
}

TDialog::~TDialog()
{

}

void TDialog::show()
{
	refresh();
}

void TDialog::touchPressEvent(TTouchEvent *e)
{
	if (isInRealArea(e->x(), e->y()))
	{
		//告诉被覆盖的对象，重绘移动后的区域
		chgInValid(NULL, NULL);
		//在区域内，且之前为失去焦点的，重新加上焦点
		if (!hasFocus)
		{
			state = true;
			hasFocus = true;
			TBufPainter p(getBuffer()->getBufAddr(),getRect());
			p.drawDialogTitle(0, 0, width(), getName(), true);
			refresh();
		}
		if ( e->x() < (x() + width()) &&  e->y() < (y() + WIN_TITLE_H))
		{
			state = true;
			contactOffsetW = e->x() - x();
			contactOffsetH = e->y() - y();
			preX = x();
			preY = y();
		}
		if (getInvalidList())//被激活，且有覆盖区
		{
			TWidget* tmp;
			if (getInvalidList()->getFirst()->getParents() != this)//若不是子类  则清理覆盖区
				getInvalidList()->remove();
			while (tmp = getInvalidList()->getNext())
				if (tmp->getParents() != this)//若不是子类  则清理覆盖区
					getInvalidList()->remove();
			if (getChildList())//这里只查看子类的没有递归查看子类----------------------------------------
			{
				tmp = (TWidget*)getChildList()->getFirst();
				if(tmp->getInvalidList())
				tmp->getInvalidList()->clear();
				while (tmp = (TWidget*)getChildList()->getNext())
					if (tmp->getInvalidList())
					tmp->getInvalidList()->clear();
			}
			showAll();//重绘  -------保证在最顶端
		}
	}
	else if(hasFocus)
	{
		hasFocus = false;
		TBufPainter p(getBuffer()->getBufAddr(),getRect());
		p.drawDialogTitle(0, 0, width(), getName(), false);
		refresh();
	}
};

void TDialog::touchReleaseEvent(TTouchEvent *e)
{
	state = false;
	contactOffsetW = -1;
	contactOffsetH = -1;
};

void TDialog::touchMoveEvent(TTouchEvent *e)
{
	if (state && contactOffsetW != -1)
	{ 
		int32 nowX = e->x() - contactOffsetW;
		int32 nowY = e->y() - contactOffsetH;
		TRect r1(preX, preY, width(), height());//两个矩形用来 收集移动后留下的空隙
		TRect r2(nowX, nowY, width(), height());
		//改变自己的坐标
		getRect()->moveTopLeft(nowX, nowY);
		//遍历改变子类的坐标 
		chgChildsXY(this);
		//遍历画全部的子类的图像
		showAll();
		//遍历树，并设置覆盖情况  Zpos / 移动后面补充没加
		restRect(preX, preY, nowX, nowY, r1, r2);
		//告诉被覆盖的对象，重绘移动后的区域
		chgInValid(&r1, &r2);
		//改变之前坐标
		preX = nowX;
		preY =nowY;
	}
};


//移动后释放的区域  
void TDialog::restRect(int32 preX, int32 preY, int32 nowX, int32 nowY ,TRect &r1, TRect &r2)
{
	//chgInValid(&r1, NULL); //整个dialog刷新
	/*	|---------|
	|---|-----|	  |
	|	|	  |	  |
	|	|---------|
	|---------|*/
	if (nowX > preX)
	{
		if (preY > nowY)//右上方移动
		{
			r1.setLeft(preX -1);
			r1.setTop(preY -1);
			r1.setWidth(nowX - preX + 2);
			r1.setHeight(height() + nowY - preY + 2);
			r2.setLeft(preX -1);
			r2.setTop(nowY + height() - 1);
			r2.setWidth(width()+1);
			r2.setHeight(preY - nowY + 2);
		}
		else //右下方移动
		{
			r1.setLeft(preX -1);
			r1.setTop(preY -1);
			r1.setWidth(width()+1);
			r1.setHeight(nowY - preY + 2);
			r2.setLeft(preX-1);
			r2.setTop(nowY-1);
			r2.setWidth(nowX - preX + 2);
			r2.setHeight(height() + nowY - preY + 2);
		}
	}
	else
	{
		if (preY > nowY)//左上方移动
		{
			r1.setLeft(nowX + width() - 1);
			r1.setTop(preY-1);
			r1.setWidth(preX - nowX + 2);
			r1.setHeight(nowY + height() - preY + 2);
			r2.setLeft(preX-1);
			r2.setTop(nowY + height() - 1);
			r2.setWidth(width()+1);
			r2.setHeight(preY - nowY + 2);
		}
		else   //左下发移动
		{
			r1.setLeft(preX-1);
			r1.setTop(preY-1);
			r1.setWidth(width()+1);
			r1.setHeight(nowY - preY +2);
			r2.setLeft(nowX + width() - 1);
			r2.setTop(nowY-1);
			r2.setWidth(preX - nowX + 2);
			r2.setHeight(preY + height() - nowY + 2);
		}
	}
}


