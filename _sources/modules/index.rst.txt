Modules
=======

Modules are intended to be code segments that perform unique operations and
functions. In general, the math, mesh and other utilities should serve
most of the needs of the modules and where appropriate functionalities of
some modules can eventually be incorporated into the utilities category.

An additional function of modules is to allow us to segregate open source
modules from export controlled or classified software.

Linear Boltzmann Solvers
------------------------

- :doc:`linear_boltzmann_solvers/solver_bases`
- :doc:`linear_boltzmann_solvers/utilities`
- :doc:`linear_boltzmann_solvers/groupsets`
- :doc:`linear_boltzmann_solvers/bcs`
- :doc:`linear_boltzmann_solvers/field_functions`
- :doc:`linear_boltzmann_solvers/executors`
- :doc:`linear_boltzmann_solvers/lua_functions`

.. toctree::
   :maxdepth: 1
   :hidden:
   :caption: Modules

   linear_boltzmann_solvers/solver_bases
   linear_boltzmann_solvers/utilities
   linear_boltzmann_solvers/groupsets
   linear_boltzmann_solvers/bcs
   linear_boltzmann_solvers/field_functions
   linear_boltzmann_solvers/executors
   linear_boltzmann_solvers/lua_functions


Point Reactor Kinetics
----------------------

- :doc:`point_reactor_kinetics/rpk`


.. toctree::
   :maxdepth: 1
   :hidden:
   :caption: Modules

   point_reactor_kinetics/rpk
