texture <- null;

function onScriptInit() {
  print("TEST from Squirrel, YAY! Also 2 + 2 = " + (2+2));
  texture = assetRegister("test", "test.bmp", Asset_Texture);
  local name = assetGetName(texture);
  print(name);

  local data = assetGetContent(texture);

  if(data == false) {
    print("We forgot to load the texture into the memory!\n");
    assetLoad(texture);
  }

  data = assetGetContent(texture);
  local size = assetGetSize(texture);

  for(local i = 0; i < size; i++) {
    print(data[i]);
  }
}
