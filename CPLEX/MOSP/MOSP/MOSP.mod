/*********************************************
 * OPL 12.8.0.0 Model
 * Author: Tamer Abdelmaguid
 * Creation Date: 1 Jan 2020 at 17:05:26
 *********************************************/

  int nj = ...;         // number of jobs
 range J = 1..nj;      // set of jobs

 int nm = ...;         // total number of machines
 range M = 1..nm;      // set of all machines in the shop
 int MW[M] = ...;      // workstation # to which machine m belongs

 int no = ...;         // total number of operations
 range O = 1..no;      // set of all operations
 int OJ[O] = ...;      // index of the job to which the operation belongs
 int OW[O] = ...;      // index of the workstation to which the operation belongs

 tuple TOpMc {
   int o;    // operation index
   int m;    // machine index
 };
 setof(TOpMc) Process = { <o, m> | o in O, m in M : OW[o] == MW[m] };
 float d[Process] = ...;         // processing times of operations on machines

 tuple TJOpPair {
   int w;    // index of the first operation
   int q;    // index of the second operation
 };
 setof(TJOpPair) DefX = { <w, q> | w, q in O : OJ[w] == OJ[q] && q > w};
 dvar int x[DefX] in 0..1;

 dvar int y[Process] in 0..1;

 tuple TOpPairMc {
   int j;   // index of the first operation
   int k;   // index of the second operation
   int m;   // machine index
 };
 setof(TOpPairMc) DefZ = { <j, k, m> | j, k in O, m in M : j != k && OW[j] == OW[k] && OW[j] == MW[m]};
 dvar int z[DefZ] in 0..1;

 dvar float+ s[O];
 dvar float+ c[J];
 dvar float Cmax;
 float bigM = ...;

 minimize Cmax;     // (1)

 subject to {
  Constraint1:
   forall(j in J) Cmax >= c[j];           // (2)

  Constraint2:
   forall(j in J)
     forall(o in O : OJ[o] == j)
       c[j] >= s[o] + sum(m in M : OW[o] == MW[m]) d[<o, m>] * y[<o, m>];        // (3)

  Constraint3:
   forall(o in O)
      sum(m in M : OW[o] == MW[m]) y[<o, m>] == 1;          // (4)

  Constraint4:
   forall(j in J)
     forall(w in O : OJ[w] == j)
       forall(r in O : OJ[r] == j && r > w)
         s[r] - s[w] >= sum(m in M : OW[w] == MW[m]) d[<w, m>] * y[<w, m>]
                        - bigM * (1 - x[<w, r>]);                                  // (5)

  Constraint5:
   forall(j in J)
     forall(w in O : OJ[w] == j)
       forall(r in O : OJ[r] == j && r > w)
         s[w] - s[r] >= sum(m in M : OW[r] == MW[m]) d[<r, m>] * y[<r, m>]
                        - bigM * x[<w, r>];                                        // (6)

  Constraint6:
   forall(j in O)
     forall(k in O : j < k)
       forall(m in M : OW[j] == MW[m] && OW[k] == MW[m])
         z[<j, k, m>] + z[<k, j, m>] <= (y[<j, m>] + y[<k, m>]) / 2;              // (7)

  Constraint9:
   forall(j in O)
     forall(k in O : j < k)
       forall(m in M : OW[j] == MW[m] && OW[k] == MW[m])
         z[<j, k, m>] + z[<k, j, m>] >= y[<j, m>] + y[<k, m>] - 1;                // (8)

  Constraint10:
   forall(j in O)
     forall(k in O : j != k && OW[k] == OW[j])
        s[k] - s[j] >= sum(m in M : OW[j] == MW[m]) d[<j, m>] * y[<j, m>]
                       - bigM * (1 - sum(m in M : OW[j] == MW[m]) z[<j, k, m>]);  // (9)
}
 