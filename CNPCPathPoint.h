

#ifndef __RGF_CNPCPATHPOINT_H_
#define __RGF_CNPCPATHPOINT_H_

class CNPCPathPoint : public CRGFComponent
{
public:
  CNPCPathPoint();
  ~CNPCPathPoint();
  void Tick();
  void Render();
private:
	geBoolean PathPoint_Frame2(const geXForm3d *XForm, geFloat DeltaTime);
	geBoolean PathPoint_Frame3(const geXForm3d *XForm, geFloat DeltaTime);

	geBitmap *PathPointTexture;
};

#endif
