--############################################### Setup mesh
nodes={}
N=100
L=2
xmin = -L/2
dx = L/N
for i=1,(N+1) do
    k=i-1
    nodes[i] = xmin + k*dx
end

meshgen1 = mesh.OrthogonalMeshGenerator.Create({ node_sets = {nodes,nodes} })
grid = mesh.MeshGenerator.Execute(meshgen1)

--############################################### Set Material IDs
mesh.SetUniformMaterialID(0)


function D_coef(i,pt)
    return 3.0 + pt.x + pt.y
end
function Q_ext(i,pt)
    return pt.x*pt.x
end
function Sigma_a(i,pt)
    return pt.x*pt.y*pt.y
end

-- Setboundary IDs
-- xmin,xmax,ymin,ymax,zmin,zmax
e_vol = logvol.RPPLogicalVolume.Create({xmin=0.99999,xmax=1000.0  , infy=true, infz=true})
w_vol = logvol.RPPLogicalVolume.Create({xmin=-1000.0,xmax=-0.99999, infy=true, infz=true})
n_vol = logvol.RPPLogicalVolume.Create({ymin=0.99999,ymax=1000.0  , infx=true, infz=true})
s_vol = logvol.RPPLogicalVolume.Create({ymin=-1000.0,ymax=-0.99999, infx=true, infz=true})

e_bndry = 0
w_bndry = 1
n_bndry = 2
s_bndry = 3

mesh.SetBoundaryIDFromLogicalVolume(e_vol,e_bndry)
mesh.SetBoundaryIDFromLogicalVolume(w_vol,w_bndry)
mesh.SetBoundaryIDFromLogicalVolume(n_vol,n_bndry)
mesh.SetBoundaryIDFromLogicalVolume(s_vol,s_bndry)

--############################################### Add material properties
--#### DFEM solver
phys1 = diffusion.DFEMSolverCreate(grid)

diffusion.DFEMSetBCProperty(phys1, "boundary_type", e_bndry, "dirichlet", 0.0)
diffusion.DFEMSetBCProperty(phys1, "boundary_type", w_bndry, "dirichlet", 0.0)
diffusion.DFEMSetBCProperty(phys1, "boundary_type", n_bndry, "dirichlet", 0.0)
diffusion.DFEMSetBCProperty(phys1, "boundary_type", s_bndry, "dirichlet", 0.0)

solver.SetBasicOption(phys1, "residual_tolerance", 1e-8)
solver.Initialize(phys1)
solver.Execute(phys1)

--############################################### Get field functions
fflist,count = solver.GetFieldFunctionList(phys1)

--############################################### Export VTU
if (master_export == nil) then
    fieldfunc.ExportToVTK(fflist[1],"CFEMDiff2D_analytic_coef","flux")
end

--############################################### Volume integrations
vol0 = logvol.RPPLogicalVolume.Create({infx=true, infy=true, infz=true})

ffvol = fieldfunc.FFInterpolationCreate(VOLUME)
fieldfunc.SetProperty(ffvol,OPERATION,OP_MAX)
fieldfunc.SetProperty(ffvol,LOGICAL_VOLUME,vol0)
fieldfunc.SetProperty(ffvol,ADD_FIELDFUNCTION,fflist[1])

fieldfunc.Initialize(ffvol)
fieldfunc.Execute(ffvol)
maxval = fieldfunc.GetValue(ffvol)

log.Log(LOG_0,string.format("Max-value=%.6f", maxval))
