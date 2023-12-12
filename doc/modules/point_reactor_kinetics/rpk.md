# Point Reactor Kinetics

This module concerns itself with the solution of the Point-Reactor Kinetics
equations:
$$
\label{Eq:1}
\frac{dn}{dt} = \frac{\beta_{eff} (\rho(t)-1)}{\Lambda_0} n(t) +
\sum_{j=0}^{J-1} \lambda_j c_j(t) + s_{ext}(t)
$$

$$
\label{Eq:2}
\frac{c_j}{dt} = \frac{\beta_j}{\Lambda_0} n(t) - \lambda_j c_j(t)
\quad for \ j=0,1,\dots,J-1
$$

where the primary unknowns are the neutron population, $ n $, and each of
the delayed-neutron precursors concentrations, $ c_j $. The reactivity,
$ \rho $ in units of , and the external source, $ s_{ext} $, are both
variable knowns/inputs, whereas the values $ \lambda_j, \beta_j ,
\Lambda_0 $, are known constants. The decay constants, $ \lambda_j $,
are in units of $ s^{-1} $ and the delayed neutron fractions,
$ \beta_j $, have no units. $ \beta_{eff} $ is the total delayed neutron
fraction,
$$
\label{Eq:3} \beta_{eff} = \sum_{j=0}^{J-1} \beta_j
$$
and $ \Lambda_0 $ is the neutron generation time.

Consult the whitepaper for this solver at `modules/point_reactor_kinetics/doc`.
