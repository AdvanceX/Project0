let gScene = new CScene();

let gAudio0 = new Audio('./music/Music-01.mp3');
let gAudio1 = new Audio('./music/Music-02.mp3');
let gAudio2 = new Audio('./music/Music-03.mp3');
let gAudio3 = new Audio('./music/Music-04.mp3');
let gAudio4 = new Audio('./music/Music-05.mp3');
let gAudio5 = new Audio('./music/Music-06.mp3');
let gAudio6 = new Audio('./music/Music-07.mp3');
let gAudio7 = new Audio('./music/Music-08.mp3');
let gAudio8 = new Audio('./music/Music-09.mp3');

let gWriter0 = new CTxtWriter(gCharMap0, './image/charset/Charset-01.png');
let gWriter1 = new CTxtWriter(gCharMap1, './image/charset/Charset-02.png');

let gSheet0 = new CSpriteSheet(gFrames0, './image/player/Player.png');
let gSheet1 = new CSpriteSheet(gFrames1, './image/scene/IceCube.png');
let gSheet2 = new CSpriteSheet(gFrames2, './image/prize/Prize.png');
let gSheet3 = new CSpriteSheet(gFrames3, './image/scene/Block.png');
let gSheet4 = new CSpriteSheet(gFrames4, './image/scene/Envir.png');
let gSheet10 = new CSpriteSheet(gFrames10, './image/enemy/Enemy-00.png');
let gSheet11 = new CSpriteSheet(gFrames11, './image/enemy/Enemy-01.png');
let gSheet12 = new CSpriteSheet(gFrames12, './image/enemy/Enemy-02.png');
let gSheet13 = new CSpriteSheet(gFrames13, './image/enemy/Enemy-03.png');
let gSheet14 = new CSpriteSheet(gFrames14, './image/enemy/Enemy-04.png');

window.addEventListener('load', function() {
	//gMaxLevel = localStorage.getItem("MaxLevel") || 0;
	gMaxLevel = gLevels.length - 1;
	gScene.Initialize('screen', 30 / 1000);
	gScene.SetObject(0, gHomeBG.Setup());
});

//主页背景
let gHomeBG = new function() {
	this.Logo = new Image();
	this.Tile = new Image();
	this.Entry = new Image();

	this.Logo.src = './image/background/BadIceCream.png';
	this.Tile.src = './image/background/Snowflake.png';
	this.Entry.src = './image/interface/Btn-Main.png';

	var FrameID = 0;
	var OffsetX = 0;
	var OffsetY = 0;
	var Timing0 = 0;
	var Timing1 = 0;
	var Period0 = 3;
	var Period1 = 12;
	var Switch = true;

	this.Setup = function() {
		gAudio0.loop = true;
		gAudio0.play();

		this.Tile.Pitch = this.Tile.width / 3;
		this.NumCol = (gScene.Width / this.Tile.Pitch) + 1;
		this.NumRow = (gScene.Height / this.Tile.height / 2) + 1;

		this.Logo.X = (gScene.Width - this.Logo.width) / 2;
		this.Logo.Y = (gScene.Height - this.Logo.height - this.Entry.height - 6) / 2;

		this.Entry.X = (gScene.Width - this.Entry.width) / 2;
		this.Entry.Y = this.Logo.Y + this.Logo.height + 6;

		return this;
	}
	this.Update = function(Duration) {
		/*No Action*/
	}
	this.Draw = function(Context) {
		Context.fillStyle = '#FAFFFF';
		Context.fillRect(0, 0, gScene.Width, gScene.Height);
		this.DrawBackground(Context);
		Context.drawImage(this.Logo, this.Logo.X, this.Logo.Y);
		Context.drawImage(this.Entry, this.Entry.X, this.Entry.Y);
		this.DrawText(Context);
	}
	this.DrawText = function(Context) {
		if((Timing1 += 1) >= Period1) {
			Timing1 = 0;
			Switch = !Switch;
		}

		if(Switch) {
			var Text = 'CLICK TO LICK';
			var X = (gScene.Width - gWriter1.GetWidth(Text)) / 2;
			var Y = this.Entry.Y + 8;
			gWriter1.Write(Context, Text, X, Y);
		}
	}
	this.DrawBackground = function(Context) {
		Timing0 += 1;
		OffsetX -= 1;
		OffsetY += 1;

		if(Timing0 >= Period0) {
			FrameID = (FrameID + 1) % 3;
			Timing0 = 0;
		}
		if(OffsetX <= -this.Tile.Pitch) {
			OffsetX = 0;
			OffsetY = 0;
		}

		var GridX = OffsetX;
		for(var i = 0; i < this.NumCol; ++i) {
			var GridY = (i % 2) ? (OffsetY - this.Tile.height) : OffsetY;
			for(var j = 0; j < this.NumRow; ++j) {
				Context.drawImage(this.Tile,
					this.Tile.Pitch * FrameID, 0, this.Tile.Pitch, this.Tile.height,
					GridX, GridY, this.Tile.Pitch, this.Tile.height);
				GridY += this.Tile.height << 1;
			}
			GridX += this.Tile.Pitch;
		}
	}
	this.OnClick = function(Event) {
		var CursorX = Event.offsetX * gScene.ScaleX;
		var CursorY = Event.offsetY * gScene.ScaleY;

		if((CursorX >= this.Entry.X) &&
			(CursorX <= (this.Entry.X + this.Entry.width)) &&
			(CursorY >= this.Entry.Y) &&
			(CursorY <= (this.Entry.Y + this.Entry.height)))
		{
			gScene.SetObject(0, gMenuLevel.Setup());
		}
	}
}
//游戏背景
let gGameBG = new function() {
	this.Frames = [
		{ U: 0, V: 0, W: 18, H: 18 },
		{ U: 18, V: 0, W: 18, H: 18 },
		{ U: 36, V: 0, W: 18, H: 18 },
		{ U: 54, V: 0, W: 18, H: 18 },
		{ U: 72, V: 0, W: 18, H: 18 },
		{ U: 0, V: 18, W: 36, H: 36 },
		{ U: 36, V: 18, W: 72, H: 36 },
		{ U: 0, V: 54, W: 72, H: 54 },
	];
	this.Texture = new Image();
	this.Texture.src = './image/scene/Ground.png';
	this.Canvas = document.createElement('canvas');
	this.Context = this.Canvas.getContext('2d');

	this.Setup = function(Level) {
		gAudio0.pause();
		gAudio1.loop = true;
		gAudio1.play();

		this.Canvas.width = gGrid.Width * Level.NumCol;
		this.Canvas.height = gGrid.Height * Level.NumRow;

		var Elements = [];
		var NumElem = 10;

		for(var i = 0; i < NumElem; ++i) {
			Elements[i] = {};
			Elements[i].Style = Math.floor(Math.random() * 7);
			Elements[i].X = Math.floor(Math.random() * this.Canvas.width);
			Elements[i].Y = Math.floor(Math.random() * this.Canvas.height);
		}

		this.Context.fillStyle = '#F2FFFF';
		this.Context.fillRect(0, 0, this.Canvas.width, this.Canvas.height);

		for(var i = 0; i < NumElem; ++i) {
			var Elem = Elements[i];
			var Frame = this.Frames[Elem.Style];
			this.Context.drawImage(this.Texture,
				Frame.U, Frame.V, Frame.W, Frame.H,
				Elem.X, Elem.Y, Frame.W, Frame.H);
		}

		return this;
	}
	this.Draw = function(Context) {
		Context.drawImage(this.Canvas, gGameMap.OffsetX, gGameMap.OffsetY);
	}
	this.Update = function(Duration) {
		/*No Action*/
	}
}
//游戏地图
let gGameMap = new function() {
	this.Width = 0;
	this.Height = 0;
	this.NumCol = 0;
	this.NumRow = 0;
	this.OffsetX = 0;
	this.OffsetY = 0;
	this.Layer0 = [];
	this.Layer1 = [];
	this.Items = [];
	this.BackItems = [];

	this.Setup = function(Level) {
		this.Items = [];
		this.OffsetX = 0;
		this.OffsetY = 0;
		this.BackItems = null;
		this.NumRow = Level.NumRow;
		this.NumCol = Level.NumCol;
		this.Width = gGrid.Width * this.NumCol;
		this.Height = gGrid.Height * this.NumRow;

		var NewSize = this.NumRow;
		var OldSize = this.Layer0.length;
		if(NewSize > OldSize) {
			for(var i = OldSize; i < NewSize; ++i) {
				this.Layer0[i] = [];
				this.Layer1[i] = [];
			}
		}

		for(var i = 0; i < Level.NumRow; ++i) {
			var Row0 = this.Layer0[i];
			var Row1 = this.Layer1[i];

			for(var j = 0; j < Level.NumCol; ++j) {
				Row0[j] = null;
				Row1[j] = null;

				switch(Level.Map[i][j]) {
					//创建障碍
					case DOMAIN: Row0[j] = true; break;
					case BARRIER: Row0[j] = new CIceCube(i, j); break;
					//创建砖块
					case BLOCK0: Row0[j] = gBlock0; break;
					case BLOCK1: Row0[j] = gBlock1; break;
					case BLOCK2: Row0[j] = gBlock2; break;
					case BLOCK3: Row0[j] = gBlock3; break;
					case BLOCK4: Row0[j] = gBlock4; break;
					case BLOCK5: Row0[j] = gBlock5; break;
					case BLOCK6: Row0[j] = gBlock6; break;
					case BLOCK7: Row0[j] = gBlock7; break;
					case BLOCK8: Row0[j] = gBlock8; break;
					case BLOCK9: Row0[j] = gBlock9; break;
					case BLOCK10: Row0[j] = gBlock10; break;
					case BLOCK11: Row0[j] = gBlock11; break;
					case BLOCK12: Row0[j] = gBlock12; break;
					case BLOCK13: Row0[j] = gBlock13; break;
					//创建环境
					case ENVIR0: Row0[j] = gEnvir0; break;
					case ENVIR1: Row0[j] = gEnvir1; break;
					case ENVIR2: Row0[j] = gEnvir2; break;
					case ENVIR3: Row0[j] = gEnvir3; break;
					case ENVIR4: Row0[j] = gEnvir4; break;
					case ENVIR5: Row0[j] = gEnvir5; break;
					case ENVIR6: Row0[j] = gEnvir6; break;
					case ENVIR7: Row0[j] = gEnvir7; break;
					case ENVIR8: Row0[j] = gEnvir8; break;
					case ENVIR9: Row0[j] = gEnvir9; break;
					//创建敌人
					case ENEMY0: Row1[j] = new CEnemy0(i, j); break;
					case ENEMY1: Row1[j] = new CEnemy1(i, j); break;
					case ENEMY2: Row1[j] = new CEnemy2(i, j); break;
					case ENEMY3: Row1[j] = new CEnemy3(i, j); break;
					case ENEMY4: Row1[j] = new CEnemy4(i, j); break;
					//创建玩家
					case PLAYER: gPlayer.Setup(i, j); break;
					//创建奖品
					case PRIZE0: this.Items.push(new CStaticItem(i, j, gPrize0)); break;
					case PRIZE1: this.Items.push(new CStaticItem(i, j, gPrize1)); break;
					case PRIZE2: this.Items.push(new CActivePrize(i, j)); break;
				}
			}
		}

		if(Level.Items && (Level.Items.length > 0))
			this.BackItems = Level.Items;

		gPlayer.UpdateView();

		return this;
	}
	this.Draw = function(Context) {
		if(gPlayer.Action == FALL)
			gPlayer.Draw(Context);

		for(var i = 0; i < this.NumRow; ++i) {
			for(var n = 0; n < this.Items.length; ++n) {
				if(this.Items[n].SrcRow == i)
					this.Items[n].Draw(Context);
			}

			for(var j = 0; j < this.NumCol; ++j) {
				var Grid = this.Layer0[i][j];
				if(Grid && (typeof Grid == 'object')) {
					Grid.X = gGrid.Width * j;
					Grid.Y = gGrid.Height * i;
					Grid.Draw(Context);
				}
			}

			for(var j = 0; j < this.NumCol; ++j) {
				var Grid = this.Layer1[i][j];
				if(Grid && Grid.Draw)
					Grid.Draw(Context);
			}

			if((gPlayer.SrcRow == i) && (gPlayer.Action != FALL))
				gPlayer.Draw(Context);
		}
	}
	this.Update = function(Duration) {
		if(gScene.KBStatus[27]) {
			gScene.SetObject(0, gMenuPause.Setup());
			gScene.SetObject(1, null);
			return;
		}

		if(this.Items.length) {
			gPrize0.Update(Duration);
			gPrize1.Update(Duration);
			gPrize2.Update(Duration);

			for(var i = 0; i < this.Items.length; ++i) {
				var Item = this.Items[i];
				if(this.DetectItem(Item))
					this.Items.splice(i, 1);
				else if(Item.Update)
					Item.Update(Duration);
			}

			for(var i = 0; i < this.NumRow; ++i) {
				for(var j = 0; j < this.NumCol; ++j) {
					var Grid = this.Layer0[i][j];
					if(Grid && Grid.Update)
						Grid.Update(Duration);

					Grid = this.Layer1[i][j];
					if(Grid && Grid.Update)
						Grid.Update(Duration);
				}
			}

			if(!this.Items.length) {
				if(this.BackItems) this.UpdateItems();
				else this.PassLevel();
			}
		}

		gPlayer.Update(Duration);
	}
	this.PassLevel = function() {
		gPlayer.SetAction(JUMP);

		if(gMaxLevel < (gCurLevel + 1))
			gMaxLevel = gCurLevel + 1;
		if(gMaxLevel >= gLevels.length)
			gMaxLevel = gLevels.length - 1;

		localStorage.setItem("MaxLevel", gMaxLevel);

		setTimeout(function() {
			gAudio7.play();
			gAudio1.pause();
			gAudio1.currentTime = 0;
			gScene.SetObject(0, gMenuPause.Setup());
			gScene.SetObject(1, null);
		}, 1600);
	}
	this.FailLevel = function() {
		gScene.SetObject(0, gMenuPause.Setup());
		gScene.SetObject(1, null);
	}
	this.UpdateItems = function() {
		var Items = this.BackItems;
		if(!Items) return;

		for(var i = 0; i < Items.length; ++i) {
			var Row = Items[i][0];
			var Col = Items[i][1];
			var Type = Items[i][2];

			switch(Type) {
				case PRIZE0: this.Items.push(new CStaticItem(Row, Col, gPrize0)); break;
				case PRIZE1: this.Items.push(new CStaticItem(Row, Col, gPrize1)); break;
				case PRIZE2: this.Items.push(new CActivePrize(Row, Col)); break;
			}
		}

		this.BackItems = null;
	}
	this.DetectItem = function(Item) {
		if((gPlayer.SrcRow == Item.SrcRow) &&
			(gPlayer.SrcCol == Item.SrcCol)) {
			gAudio6.currentTime = 0;
			gAudio6.play();
			return true;
		} else return false;
	}
	this.SetTrack = function(Elem) {
		this.Layer1[Elem.SrcRow][Elem.SrcCol] = '*';
		this.Layer1[Elem.DstRow][Elem.DstCol] = Elem;
	}
	this.RemoveTrack = function(Elem) {
		this.Layer1[Elem.SrcRow][Elem.SrcCol] = null;
	}
	this.MoveElement = function(Elem) {
		Elem.SrcRow = Elem.DstRow;
		Elem.SrcCol = Elem.DstCol;
		Elem.X = gGrid.Width * Elem.SrcCol;
		Elem.Y = gGrid.Height * Elem.SrcRow;
	}
	this.DeleteElement = function(Row, Col) {
		this.Layer0[Row][Col] = null;
	}
	this.CreateIce = function(Row, Col, DifRow, DifCol) {
		if(!gGameMap.Layer0[Row][Col] &&
			!gGameMap.Layer1[Row][Col]) {
			gGameMap.Layer0[Row][Col] = new CIceCube(Row, Col);
			setTimeout(gGameMap.CreateIce, gScene.Timeout * 2000,
				Row + DifRow, Col + DifCol, DifRow, DifCol);
		}
	}
	this.RemoveIce = function(Row, Col, DifRow, DifCol) {
		if(gGameMap.Layer0[Row][Col] instanceof CIceCube) {
			gGameMap.Layer0[Row][Col].Destroy();
			setTimeout(gGameMap.RemoveIce, gScene.Timeout * 2000,
				Row + DifRow, Col + DifCol, DifRow, DifCol);
		}
	}
	this.IsWall = function(Row, Col) {
		return this.Layer0[Row][Col];
	}
	this.IsEmpty = function(Row, Col) {
		return !(this.Layer0[Row][Col] || this.Layer1[Row][Col]);
	}
}
//暂停菜单
let gMenuPause = new function() {
	this.Item0 = new String('CONTINUE');
	this.Item1 = new String('BACK');
	this.Panel = new Image();
	this.Panel.src = './image/interface/Panel-Short.png';

	this.Setup = function() {
		this.Panel.X = (gScene.Width - this.Panel.width) / 2;
		this.Panel.Y = (gScene.Height - this.Panel.height) / 2;

		this.Item0.Width = gWriter0.GetWidth(this.Item0);
		this.Item0.Height = gCharMap0[' '].H;
		this.Item0.X = (gScene.Width - this.Item0.Width) / 2;
		this.Item0.Y = this.Panel.Y + 42;

		this.Item1.Width = gWriter0.GetWidth(this.Item1);
		this.Item1.Height = gCharMap0[' '].H;
		this.Item1.X = (gScene.Width - this.Item1.Width) / 2;
		this.Item1.Y = this.Panel.Y + 82;

		return this;
	}
	this.Draw = function(Context) {
		Context.drawImage(this.Panel, this.Panel.X, this.Panel.Y);
		gWriter0.Write(Context, this.Item0, this.Item0.X, this.Item0.Y);
		gWriter0.Write(Context, this.Item1, this.Item1.X, this.Item1.Y);
	}
	this.Update = function(Duration) {

	}
	this.OnClick = function(Event) {
		var CursorY = Event.offsetY * gScene.ScaleY;

		if((CursorY >= this.Item0.Y) &&
			(CursorY <= (this.Item0.Y + this.Item0.Height))) {
			if(gPlayer.Action == JUMP) {
				if(gCurLevel == gMaxLevel) {
					gScene.SetObject(0, gMenuLevel)
				} else {
					gScene.SetObject(0, gGameBG.Setup(gLevels[++gCurLevel]));
					gScene.SetObject(1, gGameMap.Setup(gLevels[gCurLevel]));
				}
			} else if(gPlayer.Action == FALL) {
				gScene.SetObject(0, gGameBG.Setup(gLevels[gCurLevel]));
				gScene.SetObject(1, gGameMap.Setup(gLevels[gCurLevel]));
			} else {
				gScene.SetObject(0, gGameBG);
				gScene.SetObject(1, gGameMap);
			}
		}
		else if((CursorY >= this.Item1.Y) &&
			(CursorY <= (this.Item1.Y + this.Item1.Height))) {
			gScene.SetObject(0, gMenuLevel);
		}
	}
}
//关卡菜单
let gMenuLevel = new function() {
	this.Panel = new Image();
	this.Item = new Image();
	this.Lock = new Image();
	this.Panel.src = './image/interface/Panel-High.png';
	this.Item.src = './image/interface/Btn-Item.png';
	this.Lock.src = './image/interface/Btn-Lock.png';

	this.Setup = function() {
		this.Panel.X = (gScene.Width - this.Panel.width) / 2;
		this.Panel.Y = (gScene.Height - this.Panel.height) / 2;
		this.GridWidth = (this.Panel.width - 16) / 5;
		this.GridHeight = (this.Panel.height - 16) / 8;

		var OffsetX = (this.GridWidth - this.Item.width) / 2;
		var OffsetY = (this.GridHeight - this.Item.height) / 2;
		this.Item.StartX = this.Panel.X + OffsetX + 8;
		this.Item.StartY = this.Panel.Y + OffsetY + 8;

		return this;
	}
	this.Draw = function(Context) {
		Context.fillRect(0, 0, gScene.Width, gScene.Height);
		gHomeBG.DrawBackground(Context);
		Context.drawImage(this.Panel, this.Panel.X, this.Panel.Y);

		var Index = 0;
		var Y = this.Item.StartY;
		for(i = 0; i < 8; ++i) {
			var X = this.Item.StartX;
			for(j = 0; j < 5; ++j) {
				if(Index > gMaxLevel) {
					Context.drawImage(this.Lock, Math.floor(X), Math.floor(Y));
				} else {
					var Title = String(++Index);
					var PadLeft = (this.Item.width - gWriter1.GetWidth(Title)) / 2;
					var PadDown = (this.Item.height - gWriter1.GetHeight()) / 2;
					Context.drawImage(this.Item, Math.floor(X), Math.floor(Y));
					gWriter1.Write(Context, Title, X + PadLeft, Y + PadDown);
				}
				X += this.GridWidth;
			}
			Y += this.GridHeight;
		}
	}
	this.Update = function(Duration) {

	}
	this.OnClick = function(Event) {
		var CursorX = Event.offsetX * gScene.ScaleX;
		var CursorY = Event.offsetY * gScene.ScaleY;

		var ClientX = this.Panel.X + 8;
		var ClientY = this.Panel.Y + 8;
		var ClientWidth = this.Panel.width - 8;
		var ClientHeight = this.Panel.width - 8;

		if((CursorX >= ClientX) &&
			(CursorX <= (ClientX + ClientWidth)) &&
			(CursorY >= ClientY) &&
			(CursorY <= (ClientY + ClientHeight))) {
			var Row = Math.ceil((CursorY - ClientY) / this.GridHeight);
			var Col = Math.ceil((CursorX - ClientX) / this.GridWidth);
			var Option = ((Row - 1) * 5) + (Col - 1);

			if(Option <= gMaxLevel) {
				gCurLevel = Option;
				gScene.SetObject(0, gGameBG.Setup(gLevels[Option]));
				gScene.SetObject(1, gGameMap.Setup(gLevels[Option]));
			}
		}
	}
}
//可移动物
function CMobile(Sheet, Frames, Period) {
	this.Sheet = Sheet;
	this.Frames = Frames;
	this.Period = Period;
	this.SpeedX = gGrid.Width / Period;
	this.SpeedY = gGrid.Height / Period;
}
CMobile.prototype.Setup = function(Row, Col) {
	this.FrameID = 0;
	this.Timing0 = 0;
	this.Timing1 = 0;
	this.SrcRow = Row;
	this.SrcCol = Col;
	this.DstRow = Row;
	this.DstCol = Col;
	this.Action = STAY;
	this.Orient = FRONT;
	this.X = gGrid.Width * Col;
	this.Y = gGrid.Height * Row;
}
CMobile.prototype.Draw = function(Context) {
	var State = this.Action + this.Orient;
	var Frames = this.Frames[State];

	var OffsetX = (gGrid.Width - Frames.W) / 2;
	var OffsetY = gGrid.Height - Frames.H;

	var X = this.X + OffsetX + gGameMap.OffsetX;
	var Y = this.Y + OffsetY + gGameMap.OffsetY;

	this.Sheet.Draw(Context, State, X, Y, this.FrameID);
}
CMobile.prototype.Update = function(Duration) {
	if(this.Timing1-- <= 0) {
		this.NextStep();
	} else {
		this.Move();
		this.UpdateFrame();
	}
}
CMobile.prototype.UpdateFrame = function() {
	var State = this.Action + this.Orient;
	var Frames = this.Frames[State];

	if(++this.Timing0 >= Frames.TPF) {
		this.Timing0 = 0;
		if(++this.FrameID >= Frames.FC)
			this.FrameID = 0;
	}
}
CMobile.prototype.PrepMove = function(Row, Col, Orient) {
	this.DstRow = Row;
	this.DstCol = Col;
	this.Orient = Orient;
	this.Timing1 = this.Period;
	gGameMap.SetTrack(this);
}
CMobile.prototype.Move = function() {
	if(this.DstCol != this.SrcCol) {
		if(this.Timing1 == 0) {
			gGameMap.MoveElement(this);
		} else {
			this.X += this.SpeedX * gStepsX[this.Orient];
			if(this.Timing1 == (this.Period >> 1))
				gGameMap.RemoveTrack(this);
		}
	} else if(this.DstRow != this.SrcRow) {
		if(this.Timing1 == 0) {
			gGameMap.MoveElement(this);
		} else {
			this.Y += this.SpeedY * gStepsY[this.Orient];
			if(this.Timing1 == (this.Period >> 1))
				gGameMap.RemoveTrack(this);
		}
	}
}
//玩家
function CPlayer() {
	this.Frames[RUIN + FRONT] = this.Frames[RUIN];
	this.Frames[RUIN + BACK] = this.Frames[RUIN];
	this.Frames[RUIN + LEFT] = this.Frames[RUIN];
	this.Frames[RUIN + RIGHT] = this.Frames[RUIN];
	this.Frames[FALL + FRONT] = this.Frames[FALL];
	this.Frames[FALL + BACK] = this.Frames[FALL];
	this.Frames[FALL + LEFT] = this.Frames[FALL];
	this.Frames[FALL + RIGHT] = this.Frames[FALL];
	this.Frames[JUMP + FRONT] = this.Frames[JUMP];
	this.Frames[JUMP + BACK] = this.Frames[JUMP];
	this.Frames[JUMP + LEFT] = this.Frames[JUMP];
	this.Frames[JUMP + RIGHT] = this.Frames[JUMP];
}
CPlayer.prototype = new CMobile(gSheet0, gFrames0, 6);
CPlayer.prototype.Update = function(Duration) {
	if(this.Action == JUMP)
		this.UpdateFrame();
	else if(this.Action == FALL) {
		if(this.FrameID == (this.Frames[FALL].FC - 1))
			setTimeout(gGameMap.FailLevel, 2000);
		else this.UpdateFrame();
	} else {
		this.UpdateAction();
		this.UpdateView();
		this.UpdateFrame();
	}
}
CPlayer.prototype.UpdateAction = function() {
	if(this.Injured()) {
		this.SetAction(FALL);
	} else {
		if(this.Timing1 == 0) {
			if(gScene.KBStatus[32])
				this.UseSkill();
			else this.NextStep();
		} else {
			this.Timing1--;
			this.Move();
		}
	}
}
CPlayer.prototype.UpdateView = function() {
	var VisibX = gScene.Width / 2;
	var VisibY = gScene.Height / 2;

	if(this.X > VisibX) {
		gGameMap.OffsetX = Math.max(
			VisibX - this.X, gScene.Width - gGameMap.Width);
	}
	if(this.Y > VisibY) {
		gGameMap.OffsetY = Math.max(
			VisibY - this.Y, gScene.Height - gGameMap.Height);
	}
}
CPlayer.prototype.UseSkill = function() {
	var DifRow = gStepsY[this.Orient];
	var DifCol = gStepsX[this.Orient];
	var Row = this.SrcRow + DifRow;
	var Col = this.SrcCol + DifCol;
	var Cell = gGameMap.IsWall(Row, Col);

	if(!Cell) {
		this.SetAction(CAST);
		gGameMap.CreateIce(Row, Col, DifRow, DifCol)
	} else if(Cell instanceof CIceCube) {
		this.SetAction(RUIN);
		gGameMap.RemoveIce(Row, Col, DifRow, DifCol)
	}
}
CPlayer.prototype.NextStep = function() {
	var Count = 0;
	var Codes = [83, 87, 65, 68];
	//var Codes = [40, 38, 37, 39];
	var Orients = [FRONT, BACK, LEFT, RIGHT];

	for(Count = 0; Count < 4; ++Count) {
		if(gScene.KBStatus[Codes[Count]]) {
			if(this.Orient != Orients[Count]) {
				this.Orient = Orients[Count];
				this.Timing1 = 3;
				return;
			} else {
				var Row = this.SrcRow + gStepsY[this.Orient];
				var Col = this.SrcCol + gStepsX[this.Orient];
				var Cell = gGameMap.IsWall(Row, Col);
				if(!Cell) this.SetAction(MOVE, Row, Col);
			}
			break;
		}
	}

	if(Count == 4) {
		if(this.Action != STAY)
			this.SetAction(STAY);
	}
}
CPlayer.prototype.Injured = function() {
	return gGameMap.Layer1[this.SrcRow][this.SrcCol];
}
CPlayer.prototype.SetAction = function(Action, DstRow, DstCol) {
	this.Action = Action;

	if(Action == MOVE) {
		this.DstRow = DstRow;
		this.DstCol = DstCol;
		this.Timing1 = this.Period;
	} else {
		this.FrameID = 0;
		this.Timing0 = 0;

		if(Action == CAST) {
			gAudio2.play();
			this.Timing1 = this.Period;
		} else if(Action == RUIN) {
			gAudio3.play();
			this.Timing1 = this.Period;
		} else if(Action == FALL) {
			gAudio4.play();
		} else if(Action == JUMP) {
			gAudio5.play();
		}
	}
}
//敌人0
function CEnemy0(Row, Col) {
	this.Setup(Row, Col);
	this.Action = 0;
	this.Orient = this.Locate();
}
CEnemy0.prototype = new CMobile(gSheet10, gFrames10, 18);
CEnemy0.prototype.NextStep = function() {
	var Orient = this.Orient;
	if(Orient == STAY)
		Orient = this.Locate();

	var Row = this.SrcRow + gStepsY[Orient];
	var Col = this.SrcCol + gStepsX[Orient];

	if(gGameMap.IsEmpty(Row, Col)) {
		this.PrepMove(Row, Col, Orient);
	} else {
		var Orients = [RIGHT, FRONT, LEFT, BACK];
		var Orders = { [FRONT]: 1, [BACK]: 3, [LEFT]: 2, [RIGHT]: 0 };

		var Count;
		var Order = Orders[Orient];

		for(Count = 0; Count < 3; ++Count) {
			Order = (Order + 1) % 4;
			Row = this.SrcRow + gStepsY[Orients[Order]];
			Col = this.SrcCol + gStepsX[Orients[Order]];

			if(gGameMap.IsEmpty(Row, Col)) {
				this.PrepMove(Row, Col, Orients[Order]);
				break;
			}
		}

		if(Count == 3) {
			if(this.Orient != STAY) {
				this.Timing0 = 0;
				this.FrameID = 0;
				this.Orient = STAY;
			}
		}
	}
}
CEnemy0.prototype.Locate = function() {
	if(this.SrcRow < (gGameMap.NumRow / 2)) {
		if(this.SrcCol < (gGameMap.NumCol / 2))
			return RIGHT;
		else return FRONT;
	} else {
		if(this.SrcCol < (gGameMap.NumCol / 2))
			return BACK;
		else return LEFT;
	}
}
//敌人1
function CEnemy1(Row, Col) {
	this.Setup(Row, Col);
	this.Action = 0;
	this.Orient = STAY;
}
CEnemy1.prototype = new CMobile(gSheet11, gFrames11, 18);
CEnemy1.prototype.NextStep = function() {
	if((this.Orient != STAY) && (Math.random() < 0.6)) {
		var Row = this.SrcRow + gStepsY[this.Orient];
		var Col = this.SrcCol + gStepsX[this.Orient];

		if(gGameMap.IsEmpty(Row, Col)) {
			this.PrepMove(Row, Col, this.Orient);
			return;
		}
	}

	var Orients = [FRONT, BACK, LEFT, RIGHT];
	while(Orients.length) {
		var Index = Math.random() * 10;
		Index = Math.floor(Index) % Orients.length;

		var Row = this.SrcRow + gStepsY[Orients[Index]];
		var Col = this.SrcCol + gStepsX[Orients[Index]];

		if(gGameMap.IsEmpty(Row, Col)) {
			this.PrepMove(Row, Col, Orients[Index]);
			return;
		}
	   
		Orients.splice(Index, 1);
	}
}
//敌人2
function CEnemy2(Row, Col) {
	this.Setup(Row, Col);
	this.Action = 0;
	this.Orient = STAY;
}
CEnemy2.prototype = new CMobile(gSheet12, gFrames12, 30);
CEnemy2.prototype.NextStep = function() {
	if((this.Orient != STAY) && (Math.random() < 0.6)) {
		var Row = this.SrcRow + gStepsY[this.Orient];
		var Col = this.SrcCol + gStepsX[this.Orient];

		if(gGameMap.IsEmpty(Row, Col)) {
			this.PrepMove(Row, Col, this.Orient);
			return;
		}
	}

	var Orients = [FRONT, BACK, LEFT, RIGHT];
	while(Orients.length) {
		var Index = Math.random() * 10;
		Index = Math.floor(Index) % Orients.length;

		var Row = this.SrcRow + gStepsY[Orients[Index]];
		var Col = this.SrcCol + gStepsX[Orients[Index]];

		if(gGameMap.IsEmpty(Row, Col)) {
			this.PrepMove(Row, Col, Orients[Index]);
			return;
		}

		Orients.splice(Index, 1);
	}
}
//敌人3
function CEnemy3(Row, Col) {
	this.Setup(Row, Col);
	this.Action = 0;
	this.Orient = STAY;
}
CEnemy3.prototype = new CMobile(gSheet13, gFrames13, 12);
CEnemy3.prototype.NextStep = function() {
	var Start = { X: this.SrcCol, Y: this.SrcRow };
	var End = { X: gPlayer.SrcCol, Y: gPlayer.SrcRow };
	var Nodes = FindPath(gGameMap.Layer0, gGameMap.NumRow, Start, End);

	if(!Nodes || (Nodes.length < 2)) {
		this.Orient = STAY;
	} else {
		var Row = Nodes[1].Y;
		var Col = Nodes[1].X;

		var StepX = Col - this.SrcCol;
		var StepY = Row - this.SrcRow;

		if(StepX > 0) this.Orient = RIGHT;
		else if(StepX < 0) this.Orient = LEFT;
		else if(StepY > 0) this.Orient = FRONT;
		else this.Orient = BACK;

		if(gGameMap.IsEmpty(Row, Col))
			this.PrepMove(Row, Col, this.Orient);
	}
}
//敌人4
function CEnemy4(Row, Col) {
	this.Setup(Row, Col);
	this.Action = MOVE;
	this.Orient = FRONT;
	this.Period0 = this.Period;
	this.Period1 = Math.ceil(this.Period / 3);
	this.SpeedX0 = this.SpeedX;
	this.SpeedY0 = this.SpeedY;
	this.SpeedX1 = gGrid.Width / this.Period1;
	this.SpeedY1 = gGrid.Height / this.Period1;
}
CEnemy4.prototype = new CMobile(gSheet14, gFrames14, 16);
CEnemy4.prototype.NextStep = function() {
	if(this.Action == FALL || this.Action == CAST) {
		var Row = this.SrcRow + gStepsY[this.Orient];
		var Col = this.SrcCol + gStepsX[this.Orient];

		if(!this.SetPath(Row, Col, CAST, this.Orient))
			this.SetAction(MOVE);
		return;
	}

	if(this.SrcRow == gPlayer.SrcRow) {
		var Orient = (this.SrcCol > gPlayer.SrcCol) ? LEFT : RIGHT;
		var Row = this.SrcRow
		var Col = this.SrcCol + gStepsX[Orient];
		if(this.SetPath(Row, Col, FALL, Orient)) return;
	} else if(this.SrcCol == gPlayer.SrcCol) {
		var Orient = (this.SrcRow > gPlayer.SrcRow) ? BACK : FRONT;
		var Row = this.SrcRow + gStepsY[Orient];
		var Col = this.SrcCol;
		if(this.SetPath(Row, Col, FALL, Orient)) return;
	}

	if(Math.random() < 0.6) {
		var Row = this.SrcRow + gStepsY[this.Orient];
		var Col = this.SrcCol + gStepsX[this.Orient];
		if(this.SetPath(Row, Col, MOVE, this.Orient)) return;
	}

	var Orients = [FRONT, BACK, LEFT, RIGHT];
	while(Orients.length) {
		var Index = Math.random() * 10;
		Index = Math.floor(Index) % Orients.length;
		var Orient = Orients[Index];

		var Row = this.SrcRow + gStepsY[Orient];
		var Col = this.SrcCol + gStepsX[Orient];

		if(this.SetPath(Row, Col, MOVE, Orient))
			return;

		Orients.splice(Index, 1);
	}
}
CEnemy4.prototype.SetAction = function(Action) {
	this.Action = Action;

	if(Action == MOVE) {
		this.Period = this.Period0;
		this.SpeedX = this.SpeedX0;
		this.SpeedY = this.SpeedY0;
	} else {
		this.Period = this.Period1;
		this.SpeedX = this.SpeedX1;
		this.SpeedY = this.SpeedY1;
	}

	if(Action == FALL) {
		this.FrameID = 0;
		this.Timing0 = 0;

		gAudio8.currentTime = 0;
		gAudio8.play();
	}
}
CEnemy4.prototype.SetPath = function(Row, Col, Action, Orient) {
	if(gGameMap.IsEmpty(Row, Col)) {
		this.SetAction(Action);
		this.PrepMove(Row, Col, Orient);
		return true;
	} else return false;
}
//固定物
function CFixture(Sheet, Frames) {
	this.Sheet = Sheet;
	this.Frames = Frames;
}
CFixture .prototype.Draw = function(Context) {
	var Frames = this.Frames[this.Style];

	var OffsetX = (gGrid.Width - Frames.W) / 2;
	var OffsetY = gGrid.Height - Frames.H;

	var X = this.X + OffsetX + gGameMap.OffsetX;
	var Y = this.Y + OffsetY + gGameMap.OffsetY;

	this.Sheet.Draw(Context, this.Style, X, Y, this.FrameID);
}
CFixture .prototype.Update = function(Duration) {
	var Frames = this.Frames[this.Style];
	if(++this.Timing >= Frames.TPF) {
		this.Timing = 0;
		if(++this.FrameID >= Frames.FC)
			this.FrameID = 0;
	}
}
//冰块
function CIceCube(Row, Col) {
	this.Col = Col;
	this.Row = Row;
	this.Timing = 0;
	this.FrameID = 0;
	this.Style = 'Build';
}
CIceCube.prototype = new CFixture (gSheet1, gFrames1);
CFixture .prototype.Draw = function(Context) {
	var Frames = this.Frames[this.Style];

	var OffsetX = (gGrid.Width - Frames.W) / 2;
	var OffsetY = gGrid.Height - Frames.H;
	if(this.Style == 'Clean') OffsetY += 3;

	var X = this.X + OffsetX + gGameMap.OffsetX;
	var Y = this.Y + OffsetY + gGameMap.OffsetY;

	this.Sheet.Draw(Context, this.Style, X, Y, this.FrameID);
}
CIceCube.prototype.Update = function(Duration) {
	if(this.Timing != 0) {
		this.Timing--;
		return;
	}

	var NumFrame = this.Frames[this.Style].FC;
	if(++this.FrameID == NumFrame) {
		this.FrameID = 0;

		if(this.Style != 'Still') {
			if(this.Style == 'Clean')
				gGameMap.DeleteElement(this.Row, this.Col);
			else if(this.Style == 'Build') {
				this.Style = 'Shine';
				this.Timing = 20;
			} else this.Style = 'Still';
		}
	}
}
CIceCube.prototype.Destroy = function() {
	if(this.Style != 'Clean') {
		this.FrameID = 0;
		this.Style = 'Clean';
	}
}
//奖品
function CPrize(Style) {
	this.Timing = 0;
	this.FrameID = 0;
	this.Style = Style;
}
CPrize.prototype = new CFixture (gSheet2, gFrames2);
//砖块
function CBlock(Style) {
	this.Style = Style;
}
CBlock.prototype = new CFixture (gSheet3, gFrames3);
CBlock.prototype.Update = function(Duration) {

}
//环境
function CEnvironment(Style) {
	this.Style = Style;
}
CEnvironment.prototype = new CFixture (gSheet4, gFrames4);
CEnvironment.prototype.Draw = function(Context) {
	var Frames = this.Frames[this.Style];

	var OffsetX = ((gGrid.Width * Frames.C) - Frames.W) / 2;
	var OffsetY = (gGrid.Height * Frames.R) - Frames.H;

	var X = this.X + OffsetX + gGameMap.OffsetX;
	var Y = this.Y + OffsetY + gGameMap.OffsetY;

	this.Sheet.Draw(Context, this.Style, X, Y);
}
CEnvironment.prototype.Update = function(Duration) {

}
//活跃物品
function CActiveItem(Entity, Period) {
	this.Entity = Entity;
	this.Period = Period;
	this.SpeedX = gGrid.Width / Period;
	this.SpeedY = gGrid.Height / Period;
}
CActiveItem.prototype.Setup = function(Row, Col, Orient) {
	this.Timing = 0;
	this.SrcRow = Row;
	this.SrcCol = Col;
	this.DstRow = Row;
	this.DstCol = Col;
	this.Orient = Orient;
	this.X = gGrid.Width * Col;
	this.Y = gGrid.Height * Row;
}
CActiveItem.prototype.Draw = function(Context) {
	this.Entity.X = this.X;
	this.Entity.Y = this.Y;
	this.Entity.Draw(Context);
}
CActiveItem.prototype.Update = function(Duration) {
	if(this.Timing-- > 0) this.Move();
	else this.NextStep();
}
CActiveItem.prototype.PrepMove = function(Row, Col, Orient) {
	this.DstRow = Row;
	this.DstCol = Col;
	this.Orient = Orient;
	this.Timing = this.Period;
}
CActiveItem.prototype.Move = function() {
	if(this.DstCol != this.SrcCol) {
		if(this.Timing == 0) gGameMap.MoveElement(this);
		else this.X += this.SpeedX * (this.DstCol - this.SrcCol);
	} else if(this.DstRow != this.SrcRow) {
		if(this.Timing == 0) gGameMap.MoveElement(this);
		else this.Y += this.SpeedY * (this.DstRow - this.SrcRow);
	}
}
//静态物品
function CStaticItem(Row, Col, Entity) {
	this.SrcRow = Row;
	this.SrcCol = Col;
	this.Entity = Entity;
	this.X = gGrid.Width * Col;
	this.Y = gGrid.Height * Row;
}
CStaticItem.prototype.Draw = function(Context) {
	this.Entity.X = this.X;
	this.Entity.Y = this.Y;
	this.Entity.Draw(Context);
}

//玩家实例
let gPlayer = new CPlayer();
//物品实例
let gPrize0 = new CPrize('Banana');
let gPrize1 = new CPrize('Grape');
let gPrize2 = new CPrize('Kiwi');
//砖块实例
let gBlock0 = new CBlock('Block0');
let gBlock1 = new CBlock('Block1');
let gBlock2 = new CBlock('Block2');
let gBlock3 = new CBlock('Block3');
let gBlock4 = new CBlock('Block4');
let gBlock5 = new CBlock('Block5');
let gBlock6 = new CBlock('Block6');
let gBlock7 = new CBlock('Block7');
let gBlock8 = new CBlock('Wood0');
let gBlock9 = new CBlock('Wood1');
let gBlock10 = new CBlock('Wood2');
let gBlock11 = new CBlock('Wood3');
let gBlock12 = new CBlock('Wood4');
let gBlock13 = new CBlock('Wood5');
//环境实例
let gEnvir0 = new CEnvironment("Cabin-Sum");
let gEnvir1 = new CEnvironment("Cabin-Win");
let gEnvir2 = new CEnvironment("Chimney-Sum");
let gEnvir3 = new CEnvironment("Chimney-Win");
let gEnvir4 = new CEnvironment("Column");
let gEnvir5 = new CEnvironment("SmallTree");
let gEnvir6 = new CEnvironment("SnowHouse");
let gEnvir7 = new CEnvironment("Snowman");
let gEnvir8 = new CEnvironment("Tree");
let gEnvir9 = new CEnvironment("Well");

//奖品1
function CActivePrize(Row, Col) {
	var Rand = Math.random() * 10;
	var Orient = Math.floor(Rand) % 4;
	this.Setup(Row, Col, Orient);
}
CActivePrize.prototype = new CActiveItem(gPrize2, 14);
CActivePrize.prototype.NextStep = function() {
	if(Math.random() < 0.6) {
		var Row = this.SrcRow + gStepsY[this.Orient];
		var Col = this.SrcCol + gStepsX[this.Orient];

		if(gGameMap.IsEmpty(Row, Col) && this.IsSafe(Row, Col)) {
			this.PrepMove(Row, Col, this.Orient);
			return;
		}
	}

	var Orients = [FRONT, BACK, LEFT, RIGHT];
	while(Orients.length) {
		var Index = Math.random() * 10;
		Index = Math.floor(Index) % Orients.length;

		var Row = this.SrcRow + gStepsY[Orients[Index]];
		var Col = this.SrcCol + gStepsX[Orients[Index]];

		if(gGameMap.IsEmpty(Row, Col) && this.IsSafe(Row, Col)) {
			this.PrepMove(Row, Col, Orients[Index]);
			return;
		}

		Orients.splice(Index, 1);
	}
}
CActivePrize.prototype.IsSafe = function(Row, Col) {
	if((Row == gPlayer.SrcRow) && (Col == gPlayer.SrcCol))
		return false;
	if((Row == gPlayer.DstRow) && (Col == gPlayer.DstCol))
		return false;
	return true;
}