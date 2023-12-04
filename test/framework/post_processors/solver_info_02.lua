-- Post-Processor test with lots of post-processors
-- Testing table wrapping and getting the value of a post-processor by both
-- handle and name

-- Example Point-Reactor Kinetics solver
phys0 = prk.TransientSolver.Create({ initial_source = 0.0 })

for k = 1, 20 do
  chi.SolverInfoPostProcessor.Create({
    name = "neutron_population" .. tostring(k),
    solver = phys0,
    info = { name = "neutron_population" },
    print_on = { "" }
  })
end
pp21 = chi.SolverInfoPostProcessor.Create({
  name = "neutron_population" .. tostring(21),
  solver = phys0,
  info = { name = "neutron_population" },
  print_on = { "" }
})

chi.PostProcessorPrinterSetOptions({
  time_history_limit = 5,
})

SolverInitialize(phys0)

for t = 1, 20 do
  chiSolverStep(phys0)
  time = chiSolverGetInfo(phys0, "time_next")
  print(t, string.format("%.3f %.5f",time, chiSolverGetInfo(phys0, "population_next")))

  chiSolverAdvance(phys0)
  if (time > 0.1) then
    prk.SetParam(phys0, "rho", 0.8)
  end
end

print("Manual neutron_population1=",
  string.format("%.5f", chi.PostProcessorGetValue("neutron_population1")))
print("Manual neutron_population1=",
  string.format("%.5f", chi.PostProcessorGetValue(pp21)))
