//向量点积(向量1,向量2)
function VecDot(Vec0, Vec1) {
	return (Vec0.X * Vec1.X) + (Vec0.Y * Vec1.Y);
}
//向量求长(向量)
function VecLength(Vector) {
	var SqLen = (Vector.X * Vector.X) + (Vector.Y * Vector.Y);
	return Math.sqrt(SqLen);
}
//向量单位化(向量)
function VecNormalize(Vector) {
	var Result = {};
	var RecLen = 1 / VecLength(Vector);

	Result.X = Vector.X * RecLen;
	Result.Y = Vector.Y * RecLen;

	return Result;
}

//视线跟踪(源,目标)
function GazeTrack(Origin, Target) {
	var Vector = {};
	Vector.X = Origin.X - Target.X;
	Vector.Y = Origin.Y - Target.Y;

	var RecLen = 1 / VecLength(Vector);;
	Vector.X *= RecLen;
	Vector.Y *= RecLen;

	return Vector;
}
//多边形投影(顶点数组,轴)
function PolyProject(Verts, Axis) {
	var Vector = {};
	var Dot, Min, Max;

	Vector.X = Verts[0];
	Vector.Y = Verts[1];
	Min = Max = VecDot(Axis, Vector);

	for(var i = 2; i < Verts.length - 1; i += 2) {
		Vector.X = Verts[i];
		Vector.Y = Verts[i + 1];

		Dot = VecDot(Axis, Vector);
		Min = (Dot < Min) ? Dot : Min;
		Max = (Dot > Max) ? Dot : Max;
	}

	return [Min, Max];
}

//获取矩形(对象)
function GetRect(Object) {
	var Rect = { Min: {}, Max: {} };

	Rect.Min.X = Object.X;
	Rect.Min.Y = Object.Y;
	Rect.Max.X = Object.X + Object.Width;
	Rect.Max.Y = Object.Y + Object.Height;

	return Rect;
}
//获取重叠(矩形0,矩形1)
function GetOverlay(Rect0, Rect1) {
	var Rect = { Min: {}, Max: {} };

	Rect.Min.X = Math.max(Rect0.Min.X, Rect1.Min.X);
	Rect.Min.Y = Math.max(Rect0.Min.Y, Rect1.Min.Y);
	Rect.Max.X = Math.min(Rect0.Max.X, Rect1.Max.X);
	Rect.Max.Y = Math.min(Rect0.Max.Y, Rect1.Max.Y);

	if((Rect.Min.X >= Rect.Max.X) || (Rect.Min.Y >= Rect.Max.Y))
		return null;
	else return Rect;
}

//矩形碰撞(对象0,对象1)
function RectCollided(Rect0, Rect1) {
	var Bottom0 = Math.min(Rect0.Max.X, Rect1.Max.X);
	var Top1 = Math.max(Rect0.Min.X, Rect1.Min.X);
	var Right0 = Math.min(Rect0.Max.Y, Rect1.Max.Y);
	var Left1 = Math.max(Rect0.Min.Y, Rect1.Min.Y);

	if(Bottom0 < Top1) return false;
	if(Right0 < Left1) return false;
	return true;
}
//圆形碰撞(对象0,对象1)
function CircleCollided(Circle0, Circle1) {
	var DiffX = Circle0.X - Circle1.X;
	var DiffY = Circle0.Y - Circle1.Y;
	var Limit = Circle0.Radius + Circle1.Radius;

	var SqLimit = Limit * Limit;
	var SqDist = (DiffX * DiffX) + (DiffY * DiffY);

	return SqDist <= SqLimit;
}
//多边形碰撞(对象0,对象1)
function PolyCollided(Verts0, Verts1) {
	var Index = 0;
	var Normal = {};
	var Verts = Verts0	
	var Len0 = Verts0.length;
	var Len1 = Verts1.length;
	var Length = Len0 + Len1;

	for(var i = 0; i < (Length - 1); i += 2) {
		if(i < Len0) {
			Index = i;
		} else {
			Verts = Verts1;
			Index = i - Len0;
		}

		if(i == (Verts.length - 2)) {
			Normal.Y = Verts[0] - Verts[Index]; // x1-x0
			Normal.X = Verts[Index + 1] - Verts[1]; // y0-y1
		} else {
			Normal.Y = Verts[Index + 2] - Verts[Index]; // x1-x0
			Normal.X = Verts[Index + 1] - Verts[Index + 3]; // y0-y1
		}

		var Range0 = PolyProject(Verts0, Normal);
		var Range1 = PolyProject(Verts1, Normal);

		var Dist;
		if(Range0[0] < Range1[0])
			Dist = Range1[0] - Range0[1];
		else Dist = Range0[0] - Range1[1];
		if(Dist > 0) return false;
	}

	return true;
}
//像素碰撞(对象0,对象1,离屏缓存,绘制函数)
function PixelCollided(Obj0, Obj1, OSBuff, Draw) {
	var Rect0 = GetRect(Obj0);
	var Rect1 = GetRect(Obj1);
	var Rect = GetOverlay(Rect0, Rect1);

	if(!Rect) {
		return false;
	} else {
		var X = Rect.Min.X
		var Y = Rect.Min.Y
		var Width = Rect.Max.X - Rect.Min.X;
		var Height = Rect.Max.Y - Rect.Min.Y;

		OSBuff.save();
		OSBuff.clearRect(0, 0, OSBuff.Width, OSBuff.Height);
		Draw(OSBuff, Obj0);
		OSBuff.restore();
		var Data0 = OSBuff.getImageData(X, Y, Width, Height).data;

		OSBuff.save();
		OSBuff.clearRect(0, 0, OSBuff.Width, OSBuff.Height);
		Draw(OSBuff, Obj1);
		OSBuff.restore();
		var Data1 = OSBuff.getImageData(X, Y, Width, Height).data;

		for(var i = 3; i < Data0.length; i += 4) {
			if((Data0[i] > 0) && (Data1[i] > 0))
				return true;
		}

		return false;
	}
}

//搜索路径(地图,行数,起点,终点)
function FindPath(Map, NumRow, Start, End) {
	class CNode {
		constructor(X, Y, Parent, Start) {
			this.X = X;
			this.Y = Y;
			this.SetWeight(Start);
			this.SetParent(Parent);
		}
		SetWeight(Start) {
			this.H = Math.abs(Start.X - this.X) + Math.abs(Start.Y - this.Y);
		}
		SetParent(Parent) {
			this.G = Parent ? (Parent.G + 1) : 0;
			this.F = this.G + this.H;
			this.Parent = Parent;
		}
	}

	var DirX = [1, -1, 0, 0];
	var DirY = [0, 0, 1, -1];

	var OpenList = [];
	var CloseMap = [];
	var WeightMap = [];

	for(var i = 0; i < NumRow; ++i) {
		CloseMap[i] = [];
		WeightMap[i] = [];
	}

	OpenList.push(new CNode(End.X, End.Y, null, Start));
	while(OpenList.length) {
		var Node = OpenList.shift();
		CloseMap[Node.Y][Node.X] = true;

		if((Node.X == Start.X) && (Node.Y == Start.Y)) {
			var PathNodes = [];
			while(Node) {
				PathNodes.push(Node);
				Node = Node.Parent;
			}
			return PathNodes;
		}

		for(var i = 0; i < 4; ++i) {
			var X = Node.X + DirX[i];
			var Y = Node.Y + DirY[i];

			if(!Map[Y][X] && !CloseMap[Y][X]) {
				var Node1 = WeightMap[Y][X];
				if(!Node1) {
					WeightMap[Y][X] = new CNode(X, Y, Node, Start);
					OpenList.push(WeightMap[Y][X]);
				} else {
					if((Node.G + 1) < Node1.G)
						Node1.SetParent(Node);
				}
			}
		}

		OpenList.sort((A, B) => { return A.F - B.F; })
	}

	return null;
}


//场景
function CScene() {
    this.Objects = [];
    this.Timeout = 0;

    if (!CScene.prototype.KBStatus) {
        CScene.prototype.KBStatus = [];

        window.addEventListener('keydown',
            function (Event) {
                CScene.prototype.KBStatus[Event.keyCode] = true;
                Event.preventDefault();
            },
			false);
        window.addEventListener('keyup',
            function (Event) {
                CScene.prototype.KBStatus[Event.keyCode] = false;
                Event.preventDefault();
            },
            false);
    }
    if (!CScene.prototype.MBStatus) {
        CScene.prototype.MBStatus = [];

        window.addEventListener('mousedown',
            function (Event) {
                CScene.prototype.MBStatus[Event.button] = true;
                Event.preventDefault();
            },
            false);
        window.addEventListener('mouseup',
            function (Event) {
                CScene.prototype.MBStatus[Event.button] = false;
                Event.preventDefault();
            },
            false);
    }
}
//场景::初始化(画布ID,超时时间)
CScene.prototype.Initialize = function(CanvasID, Timeout) {
	this.Canvas = document.getElementById(CanvasID);
	this.Context = this.Canvas.getContext && this.Canvas.getContext('2d');
	if(!this.Context) return alert('Please upgrade your browser.');

	this.Width = this.Canvas.width;
	this.Height = this.Canvas.height;
	this.ScaleX = this.Width / parseInt(this.Canvas.style.width);
	this.ScaleY = this.Height / parseInt(this.Canvas.style.height);
	this.Timeout = Timeout;
	
	var that = this;
	this.Update = function() {
		for(var i = 0; i < that.Objects.length; ++i) {
			if(that.Objects[i]) {
				that.Objects[i].Draw(that.Context);
				that.Objects[i].Update(that.Timeout);
			}
		}
		setTimeout(that.Update, that.Timeout * 1000);
	};
	this.Canvas.addEventListener('click',
		function(Event) {
			for(var i = 0; i < that.Objects.length; ++i) {
				if(that.Objects[i].OnClick)
					that.Objects[i].OnClick(Event);
			}
			Event.preventDefault();
		}, false);

	this.Context.imageSmoothingEnabled = false;

	if(this.Timeout != 0) this.Update();
}
//场景::设置对象(索引,对象)
CScene.prototype.SetObject = function(Index, Object) {
	this.Objects[Index] = Object;
}
//场景::移除对象(索引)
CScene.prototype.RemoveObject = function(Index) {
	this.Objects.splice(Index, 1);
}
//场景::按键状态(键码)
CScene.prototype.KeyDowned = function(KeyCode) {
	return CScene.prototype.KBStatus[KeyCode];
}


//滚动屏(速度X,速度Y,回调函数,宽,高)
function CScrollScreen(SpeedX, SpeedY, Callback, Width, Height) {
	this.OffsetX = 0;
	this.OffsetY = 0;
	this.SpeedX = SpeedX || 0.0;
	this.SpeedY = SpeedY || 0.0;
	this.Canvas = document.createElement('canvas');
	this.Context = this.Canvas.getContext('2d');
	this.Canvas.width = Width || gScene.Width;
	this.Canvas.height = Height || gScene.Height;

	if(Callback) Callback.call(this);
}
//滚动屏::更新(时长)
CScrollScreen.prototype.Update = function(Duration) {
	this.OffsetX += this.SpeedX * Duration;
	this.OffsetY += this.SpeedY * Duration;
	this.OffsetX %= this.Canvas.width;
	this.OffsetY %= this.Canvas.height;
}
//滚动屏::绘制(上下文)
CScrollScreen.prototype.Draw = function(Context) {
	if(this.SpeedX != 0.0) {
		var Width = this.Canvas.width;
		var Height = this.Canvas.height;
		var Offset = Math.floor(this.OffsetX);
		var Remain = Width - Offset;

		if(Offset > 0) {
			Context.drawImage(this.Canvas,
				Remain, 0, Offset, Height,
				0, 0, Offset, Height);
		}
		if(Remain > 0) {
			Context.drawImage(this.Canvas,
				0, 0, Remain, Height,
				Offset, 0, Remain, Height);
		}
	}
	else if(this.SpeedY != 0.0) {
		var Width = this.Canvas.width;
		var Height = this.Canvas.height;
		var Offset = Math.floor(this.OffsetY);
		var Remain = Height - Offset;

		if(Offset > 0) {
			Context.drawImage(this.Canvas,
				0, Remain, Width, Offset,
				0, 0, Width, Offset);
		}
		if(Remain > 0) {
			Context.drawImage(this.Canvas,
				0, 0, Width, Remain,
				0, Offset, Width, Remain);
		}
	}
}


//标题板(文本,字体,坐标X,坐标Y,更新函数)
function CTitleBoard(Text, Font, X, Y, Update) {
	this.X = X;
	this.Y = Y;
	this.Text = Text;
	this.Font = Font;
	this.Color = '#FFFFFF';
	if(Update) this.Update = Update;
}
//标题板::更新(时长)
CTitleBoard.prototype.Update = function(Duration) {
	/*No Action*/
}
//标题板::绘制(上下文)
CTitleBoard.prototype.Draw = function(Context) {
	Context.font = this.Font;
	Context.fillStyle = this.Color;
	Context.textAlign = 'center';

	var CenterX = gScene.Width / 2;
	var CenterY = gScene.Height / 2;

	Context.fillText(this.Text, CenterX + this.X, CenterY + this.Y);
}


//精灵表单(精灵信息,图像源,回调函数)
function CSpriteSheet(Frames, Source, Callback) {
	if(arguments.length != 0)
		this.Setup(Frames, Source, Callback);
}
//精灵表单::设置(单元信息,图像源,回调函数)
CSpriteSheet.prototype.Setup = function(Frames, Source, Callback) {
	this.Frames = Frames;
	this.Texture = new Image();
	this.Texture.onload = Callback;
	this.Texture.src = Source;
}
//精灵表单::绘制(上下文,精灵ID,坐标X,坐标Y,帧号)
CSpriteSheet.prototype.Draw = function(Context, Subset, X, Y, Frame) {
	if(!Frame) Frame = 0;
	var Sprite = this.Frames[Subset];

	Context.drawImage(this.Texture,
		Sprite.U + (Sprite.W * Frame), Sprite.V, Sprite.W, Sprite.H,
		Math.floor(X), Math.floor(Y), Sprite.W, Sprite.H);
}


//精灵组
function CSpriteGroup() {
	this.Members = [];
	this.Dustbin = [];
	this.Count = {};
}
//精灵组::添加(对象)
CSpriteGroup.prototype.Add = function(Object) {
	Object.Group = this;
	this.Members.push(Object);
	if(Object.Type) this.Count[Object.Type] =
		(this.Count[Object.Type] || 0) + 1;
	return Object;
}
//精灵组::删除(对象)
CSpriteGroup.prototype.Delete = function(Object) {
	this.Dustbin.push(Object);
}
//精灵组::重置回收站
CSpriteGroup.prototype.ResetDustbin = function() {
	this.Dustbin = [];
}
//精灵组::清空回收站
CSpriteGroup.prototype.ClearDustbin = function() {
	for(var i = 0; i < this.Dustbin.length; ++i) {
		var Index = this.Members.indexOf(this.Dustbin[i]);
		if(Index != -1) {
			this.Members.splice(Index, 1);
			if(this.Dustbin[i].Type)
				this.Count[this.Dustbin[i].Type]--;
		}
	}
}
//精灵组::分派(函数名)
CSpriteGroup.prototype.Dispatch = function(FuncName) {
	var Params = Array.prototype.slice.call(arguments, 1);
	for(var i = 0; i < this.Members.length; ++i) {
		var Object = this.Members[i];
		Object[FuncName].apply(Object, Params);
	}
}
//精灵组::匹配(函数)
CSpriteGroup.prototype.Match = function(Function) {
	for(var i = 0; i < this.Members.length; ++i) {
		if(Function.call(this.Members[i]))
			return this.Members[i];
	}
	return null;
}
//精灵组::更新(时长)
CSpriteGroup.prototype.Update = function(Duration) {
	this.ResetDustbin();
	this.Dispatch('Update', Duration);
	this.ClearDustbin();
}
//精灵组::绘制(上下文)
CSpriteGroup.prototype.Draw = function(Context) {
	this.Dispatch('Draw', Context);
}
//精灵组::碰撞(目标,类型)
CSpriteGroup.prototype.Collide = function(Object, Type) {
	return this.Match(function() {
		if(Object != this) {
			var Result = (!Type || (this.Type & Type)) && RectCollided(Object, this);
			return Result ? this : null;
		}
	});
}


//精灵(精灵表单,类型,属性)
function CSprite(Sheet, Class, Props) {
	if(arguments.length > 0)
		this.Setup(Sheet, Class, Props);
}
//精灵::设置(精灵表单,类型,属性)
CSprite.prototype.Setup = function(Sheet, Class, Props) {
	this.Sheet = Sheet;
	this.Class = Class;
	this.Extend(Props);
	this.Frame = this.Frame || 0;
	this.Width = Sheet.Frames[Class].Width;
	this.Height = Sheet.Frames[Class].Height;
}
//精灵::扩展(属性)
CSprite.prototype.Extend = function(Props) {
	if(Props) {
		for(var Prop in Props)
			this[Prop] = Props[Prop];
	}
}
//精灵::绘制(上下文)
CSprite.prototype.Draw = function(Context) {
	this.Sheet.Draw(Context, this.Class, this.X, this.Y, this.Frame);
}
//精灵::伤害(攻击力)
CSprite.prototype.Damage = function(Power) {
	this.Group.Delete(this);
}


//基本按钮(标题,坐标X,坐标Y,宽度,高度)
function CBasicButton(Title, X, Y, Width, Height) {
	if(arguments.length != 0)
		this.Setup(Title, X, Y, Width, Height);
}
//基本控件::设置(标题,坐标X,坐标Y,宽度,高度)
CBasicButton.prototype.Setup = function(Title, X, Y, Width, Height) {
	this.X = X;
	this.Y = Y;
	this.Title = Title;
	this.Width = Width;
	this.Height = Height;
	this.Pressed = false;
	this.Alpha0 = this.Alpha0 || 0.8;
	this.Alpha1 = this.Alpha1 || 0.6;
	this.BGColor = this.BGColor || '#BBBBBB';
	this.TxtColor = this.TxtColor || '#FFFFFF';
	this.TxtSize = this.TxtSize || Math.min(Width, Height) * 3 / 4;
}
//基本控件::绘制(上下文)
CBasicButton.prototype.Draw = function(Context) {
	Context.globalAlpha = this.Pressed ? this.Alpha0 : this.Alpha1;
	Context.fillStyle = this.BGColor;
	Context.fillRect(this.X, this.Y, this.Width, this.Height);

	Context.globalAlpha = 1.0;
	Context.textAlign = 'center';
	Context.textBaseline = 'middle';
	Context.font = 'bold ' + this.TxtSize + 'px arial';
	Context.fillStyle = this.TxtColor;
	Context.fillText(this.Title, this.X + (this.Width >> 1), this.Y + (this.Height >> 1));
};


//文本绘制工具(字符映射,纹理路径)
function CTxtWriter(CharMap, BitmapURL) {
	if(arguments.length != 0)
		this.Setup(CharMap, BitmapURL);
}
//文本绘制工具::设置(字符映射,纹理路径)
CTxtWriter.prototype.Setup = function(CharMap, BitmapURL) {
	this.CharMap = CharMap;
	this.Texture = new Image();
	this.Texture.src = BitmapURL;
}
//文本绘制工具::绘制(上下文,文本,坐标X,坐标Y)
CTxtWriter.prototype.Write = function(Context, Text, X, Y) {
	for(var i = 0; i < Text.length; ++i) {
		var Char = Text.charAt(i);
		var Map = this.CharMap[Char];
		Map.H = this.CharMap[' '].H;

		Context.drawImage(this.Texture,
			Map.U, Map.V, Map.W, Map.H,
			Math.floor(X), Math.floor(Y), Map.W, Map.H);

		X += Map.W;
	}
}
//文本绘制工具::获取宽度(文本)
CTxtWriter.prototype.GetWidth = function(Text) {
	var Width = 0;
	for(var i = 0; i < Text.length; ++i) {
		var Char = Text.charAt(i);
		var Map = this.CharMap[Char];
		Width += Map.W;
	}

	return Width;
}
//文本绘制工具::获取高度
CTxtWriter.prototype.GetHeight = function() {
	return this.CharMap[' '].H;
}


//音频效果(上下文,缓冲区)
function CAudioEffect(Context, Buffer) {
	this.Context = Context;
	this.Source = Context.createBufferSource();
	this.Source.buffer = Buffer;
	this.Nodes = [];
}
//音频效果::输出
CAudioEffect.prototype.Output = function() {
	for(var i = 0; i < this.Nodes.length; ++i) {
		var Node = this.Source;
		for(var j = 0; j < this.Nodes[i].length; ++j) {
			Node.connect(this.Nodes[i][j]);
			Node = this.Nodes[i][j];
		}
		Node.connect(this.Context.destination);
	}
}
//音频效果::停止
CAudioEffect.prototype.Stop = function() {
	this.Source.stop();
}
//音频效果::播放(是否循环)
CAudioEffect.prototype.Play = function(Loop) {
	this.Source.loop = Loop || false;
	this.Source.start(0);
}
//音频效果::添加节点(节点,组ID)
CAudioEffect.prototype.AddNode = function(Node, GroupID) {
	GroupID = GroupID || 0;
	this.Nodes[GroupID] = this.Nodes[GroupID] || [];
	this.Nodes[GroupID].push(Node);
}


//音频管理器
function CAudioManager() {
	this.URLs = [];
	this.Items = [];
	this.Context = new AudioContext();
}
//音频管理器::加载资源
CAudioManager.prototype.LoadResources = function() {
	var that = this;
	for(var i = 0; i < this.URLs.length; ++i) {
		LoadAudio(this.Context, this.URLs[i], Buffer => {
			var Item = new CAudioEffect(that.Context, Buffer);
			that.Items.push(Item);
		});
	}
}
//音频管理器::加载音频(上下文,路径,成功回调,失败回调)
CAudioManager.prototype.LoadAudio = function(Context, URL, FuncSucc, FuncFail) {
	var Request = new XMLHttpRequest();
	Request.open('get', URL, true);
	Request.responseType = 'arraybuffer';
	Request.onload = function() {
		Context.decodeAudioData(this.response,
			Buffer => { FuncSucc && FuncSucc(Buffer) },
			() => { FuncFail && FuncFail() }
		);
	}
	Request.send();
}
//音频管理器::创建节点(类型,参数)
CAudioManager.prototype.CreateNode = function(Type, Params) {
	switch(Type) {
		case 1: return this.Context.createPanner();
		case 3: return this.Context.createGainNode();
		case 2: return this.Context.createJavaScriptNode(Params[0], Params[1], Params[2]);	
		default: return null;
	}
}