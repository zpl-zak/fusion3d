texture <- null;

function onScriptInit() {
  print("TEST from Squirrel, YAY! Also 2 + 2 = " + (2+2));
  texture = assetRegister("test", "test.bmp", Asset_Texture);
  local name = assetGetName(texture);
  print(name);

  local data = assetGetContent(texture);

  local pk3 = assetRegister("pk3", "brutality.pk3", Asset_None);
  assetLoad(pk3);

  if(data == false) {
    print("We forgot to load the texture into the memory!\n");
    assetLoad(texture);
  }

  async(function () {
    print("Yay! Second thread rockzz!!");
  }, function (data) {
    print(data);
  })
}
