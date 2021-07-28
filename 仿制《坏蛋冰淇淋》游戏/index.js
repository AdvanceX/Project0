var CUrl = require('url');
var CHttp = require('http');
var CMime = require('mime');
var CPath = require('path');
var CFileSys = require('fs');


CHttp.createServer((Request, Response) => {
    if(Request.url != '/favicon.ico'){
        var Path = CUrl.parse(Request.url).pathname;
        if (Path == '/') Path = '/index.html';
        Path = './static_res' + Path;
        
        CFileSys.readFile(Path, (Error, Data) =>{
            if(Error){
                Response.writeHead(404, { 'Content-Type': 'text/html; charset=utf-8' });
                Response.end('页面不存在!');
            }else{
                var ExtName = CPath.extname(Path);
                var MimeType = CMime.getType(ExtName);

                Response.writeHead(200, { 'Content-Type': MimeType + '; charset=utf-8' });
                Response.end(Data);
            }
        });        
    }
}).listen(1234, () => console.log('Server running at /:1234'));