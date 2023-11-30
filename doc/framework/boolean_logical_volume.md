# BooleanLogicalVolume

## Additional Example A
The code below defines a logical volume that is within logical volume 1 but
outside logical volume 2

```
mesh.BooleanLogicalVolume.Create
({
  params = {{true, lv1}, {false, lv2}}
})
```

## Additional Example B

```
chiMeshHandlerCreate()

mesh={}
N=40
L=5
xmin = -L/2
dx = L/N
for i=1,(N+1) do
  k=i-1
  mesh[i] = xmin + k*dx
end
chiMeshCreateUnpartitioned3DOrthoMesh(mesh,mesh,mesh)
chiVolumeMesherExecute();

lv1 = mesh.SphereLogicalVolume.Create({r = 1.3, x=1.0, y=-1.0, z=2.0})

lv2 = mesh.RCCLogicalVolume.Create({r = 1.3,
                                    x0=-0.8, y0=-0.8, z0=-1.5,
                                    vx=1.0, vy=1.0, vz=3.0})
lv3 = mesh.BooleanLogicalVolume.Create
({
  parts = { { op=true, lv=lv2 },
            { op=false, lv=lv1 } }
})
chiVolumeMesherSetProperty(MATID_FROMLOGICAL, lv3, 1)

chiMeshHandlerExportMeshToVTK("lv_boolean_test1")
```

![](/images/framework/mesh/logical_volume/lv_boolean_test1.png)
