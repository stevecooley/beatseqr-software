inlets=2;
outlets=1;

var a;
var b;

function list()
{
a = args(0);
b=args(1);

outlet(0, "/beatseqr/matrix/"+a+"/"+b);

}