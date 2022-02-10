using QPALM
using LinearAlgebra
using SparseArrays
using Random

# Generate a random QP
Random.seed!(0)
n, m = 10, 20
act = 5
F = randn(n, n-1)
Q = sparse(F*F' + 1e-3*I)
A = sparse(randn(m, n))
x_star = randn(n)
y_star = [rand(act); zeros(m-act)]
q = -Q*x_star - A'*y_star
b = [A[1:act, :]*x_star; A[act+1:end, :]*x_star + rand(m-act)]

# Set up the QPALM problem
model = QPALM.Model()
params = Dict{Symbol,Any}(:max_iter=>100)
QPALM.setup!(model, Q=Q, q=q, A=A, bmax=b; params...)

# Solve the problem
results = QPALM.solve!(model)

# Print the results
println(results.info.status)
println(results.x)
println(results.y)
