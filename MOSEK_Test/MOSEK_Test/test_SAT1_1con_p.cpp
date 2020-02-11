///**********ONLY ONE constraint
//
//tetrahedron s_0 s_1 s_2 s_3 : static tetrahedron
//tetrahedron r_0 r_1 r_2 r_3 : rest pose tetrahedron
//tetrahedron p_0 p_1 p_2 p_3 : deformed pose tetrahedron
//
//minimize  p_0x^2 + p_0x p_1x + p_1x^2 +  p_0x p_2x  + p_1x p_2x + p_2x^2 + p_0x p_3x + p_1x p_3x + p_2x p_3x + p_3x^2
//+ p_0y^2 + p_0y p_1y + p_1y^2 +  p_0y p_2y  + p_1y p_2y + p_2y^2 + p_0y p_3y + p_1y p_3y + p_2y p_3y + p_3y^2
//+ p_0z^2 + p_0z p_1z + p_1z^2 +  p_0z p_2z  + p_1z p_2z + p_2z^2 + p_0z p_3z + p_1z p_3z + p_2z p_3z + p_3z^2
//
//- p_0x(2r_0x + r_1x + r_2x + r_3x)
//- p_1x(r_0x + 2r_1x + r_2x + r_3x)
//- p_2x(r_0x + r_1x + 2r_2x + r_3x)
//- p_3x(r_0x + r_1x + r_2x + 2r_3x)
//
//- p_0y(2r_0y + r_1y + r_2y + r_3y)
//- p_1y(r_0y + 2r_1y + r_2y + r_3y)
//- p_2y(r_0y + r_1y + 2r_2y + r_3y)
//- p_3y(r_0y + r_1y + r_2y + 2r_3y)
//
//- p_0z(2r_0z + r_1z + r_2z + r_3z)
//- p_1z(r_0z + 2r_1z + r_2z + r_3z)
//- p_2z(r_0z + r_1z + 2r_2z + r_3z)
//- p_3z(r_0z + r_1z + r_2z + 2r_3z)
//
//+ r_0x^2 + r_0x r_1x + r_1x^2 +  r_0x r_2x + r_1x r_2x + r_2x^2 + r_0x r_3x + r_1x r_3x + r_2x r_3x + r_3x^2
//+ r_0y^2 + r_0y r_1y + r_1y^2 +  r_0y r_2y + r_1y r_2y + r_2y^2 + r_0y r_3y + r_1y r_3y + r_2y r_3y + r_3y^2
//+ r_0z^2 + r_0z r_1z + r_1z^2 +  r_0z r_2z + r_1z r_2z + r_2z^2 + r_0z r_3z + r_1z r_3z + r_2z r_3z + r_3z^2
//
//s.t		a p_0x + b p_0y+ c p_0z >= d
//
//where
//(a,b,c) is the face normal of s_1, s_2, s_3
//d = a s_1x + b s_1y + c s_1z;
//*/
//
//
//#include <stdio.h>
//#include <iostream>
//
//
//
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp> //for matrices
//#include <glm/gtc/type_ptr.hpp>
//
//#include "mosek.h" /* Include the MOSEK definition file. */
//#include <math.h>
//
//#define NUMCON 1   /* Number of constraints.             */
//#define NUMVAR 3   /* Number of variables.               */
//#define NUMANZ 3   /* Number of non-zeros in A.          */
//#define NUMQNZ 3   /* Number of non-zeros in Q.          */
//
//glm::vec3 sT[4];	//static tetrahedron position
//glm::vec3 pT[4];	//deformed pose tetrahedron position
//glm::vec3 rT[4];	//rest pose tetrahedron position
//
//glm::vec3 rTSum;	//the sum of each coordinate values of rest pose tet. 
//glm::vec3 rConstFixed;	//Cosntant term for the obj function
//
///* This function prints log output from MOSEK to the terminal. */
//static void MSKAPI printstr(void       *handle,
//	const char str[])
//{
//	printf("%s", str);
//} /* printstr */
//
//void faceCalculation(glm::vec3 face[3], glm::vec3 *normal, double *d){
//	glm::vec3 v01 = face[1] - face[0];
//	glm::vec3 v02 = face[2] - face[0];
//
//	(*normal) = glm::normalize(glm::cross(v01, v02));
//	*(d) = glm::dot(*normal, face[0]);
//}
//
//void init(){
//	//static tetrahedron position
//	sT[0] = glm::vec3(0.0, 0.0, 0.0);
//	sT[1] = glm::vec3(1.0, 0.0, 0.0);
//	sT[2] = glm::vec3(0.0, 1.0, 0.0);
//	sT[3] = glm::vec3(0.0, 0.0, 1.0);
//
//	//rest pose tetrahedron position
//	rT[0] = glm::vec3(0.2, 0.2, 0.2);
//	rT[1] = glm::vec3(1.2, 0.2, 0.2);
//	rT[2] = glm::vec3(0.2, 1.2, 0.2);
//	rT[3] = glm::vec3(0.7, 0.7, 1.2);
//
//	//deformed pose tetrahedron position same as the rest pose for the initial value
//	pT[0] = rT[0];
//	pT[1] = rT[1];
//	pT[2] = rT[2];
//	pT[3] = rT[3];
//
//	//precompute the sum of coordinate values of rest pose tet. 
//	rTSum = glm::vec3(0.0, 0.0, 0.0);
//	for (int i = 0; i < 4; i++){
//		rTSum.x += rT[i].x;
//		rTSum.y += rT[i].y;
//		rTSum.z += rT[i].z;
//
//	}
//	std::cout << "rTSum: (" << (rTSum).x << "," << (rTSum).y << "," << (rTSum).z << ")" << std::endl;
//	
//	//constant value
//
//	rConstFixed = glm::vec3(0.0, 0.0, 0.0);
//	for (int i = 0; i < 4; i++){
//		for (int j = 0; j < (i + 1); j++){
//			rConstFixed.x += rT[i].x * rT[j].x;
//			rConstFixed.y += rT[i].y * rT[j].y;
//			rConstFixed.z += rT[i].z * rT[j].z;
//		}
//	}
//	std::cout << "rConstFixed: (" << (rConstFixed).x << "," << (rConstFixed).y << "," << (rConstFixed).z << ")" << std::endl;
//}
//void cFixedCalculation(float *cfixed){
//	(*cfixed) = 
//		(float)(  pT[1].x * pT[1].x + pT[1].x * pT[2].x + pT[2].x * pT[2].x + pT[1].x * pT[3].x + pT[2].x * pT[3].x + pT[3].x * pT[3].x 
//		+ pT[1].y * pT[1].y + pT[1].y * pT[2].y + pT[2].y * pT[2].y + pT[1].y * pT[3].y + pT[2].y * pT[3].y + pT[3].y * pT[3].y 
//		+ pT[1].z * pT[1].z + pT[1].z * pT[2].z + pT[2].z * pT[2].z + pT[1].z * pT[3].z + pT[2].z * pT[3].z + pT[3].z * pT[3].z 
//
//		- (pT[1].x * (rTSum.x + rT[1].x) + pT[2].x * (rTSum.x + rT[2].x) + pT[3].x * (rTSum.x + rT[3].x))
//		- (pT[1].y * (rTSum.y + rT[1].y) + pT[2].y * (rTSum.y + rT[2].y) + pT[3].y * (rTSum.y + rT[3].y))
//		- (pT[1].z * (rTSum.z + rT[1].z) + pT[2].z * (rTSum.z + rT[2].z) + pT[3].z * (rTSum.z + rT[3].z))
//
//		+rConstFixed.x + rConstFixed.y + rConstFixed.z);
//}
//int main(int argc, const char *argv[])
//{
//	init();
//
//	glm::vec3 face[3] = { sT[1], sT[2], sT[3] };
//	glm::vec3 normal;
//	double constraintValue;
//	float cfixed;
//
//	faceCalculation(face, &normal, &constraintValue);
//	cFixedCalculation(&cfixed);
//
//	//linear part of the obj function
//	double c[] = {  -(rTSum.x + rT[0].x) + pT[1].x + pT[2].x + pT[3].x,
//					-(rTSum.y + rT[0].y) + pT[1].y + pT[2].y + pT[3].y,
//					-(rTSum.z + rT[0].z) + pT[1].z + pT[2].z + pT[3].z };
//
//	/* Bounds on Constraints */
//	// one constraints with low bound
//	MSKboundkeye bkc[] = { MSK_BK_LO };
//	// one constraints with low bound value d
//	double       blc[] = { constraintValue };
//	// one constraints, upper bound value
//	double       buc[] = { +MSK_INFINITY };
//
//	/* Bounds on Variables */
//	// 3 variables with free bound
//	MSKboundkeye bkx[] ={ MSK_BK_FR, MSK_BK_FR, MSK_BK_FR };
//	// 3 variables  with free bound
//	double       blx[] = {-MSK_INFINITY, -MSK_INFINITY, -MSK_INFINITY	};
//	// 3 variables  with free bound
//	double       bux[] = { +MSK_INFINITY, +MSK_INFINITY, +MSK_INFINITY	};
//
//	/* Below is the sparse representation of the A
//	matrix stored by column. */
//	MSKint32t	aptrb[] = { 0,  1, 2 },  //size: #of colum, begin index
//		aptre[] = { 1,2,3 }, //size: #of colum, end index +1
//		asub[] = {	0, //size: #of nonzeros,  array of row index in A that has nonzero values
//					0,
//					0,
//				 };
//	double      aval[] = { normal.x, //size: #of nonzeros,  array of nonzero values in A
//							normal.y,
//							normal.z	
//						  };
//
//	MSKint32t     qsubi[NUMQNZ];
//	MSKint32t     qsubj[NUMQNZ];
//	double        qval[NUMQNZ];
//
//	MSKint32t     i, j;
//	double        xx[NUMVAR];
//
//	MSKenv_t      env = NULL;
//	MSKtask_t     task = NULL;
//	MSKrescodee   r;
//
//
//	/* Create the mosek environment. */
//	r = MSK_makeenv(&env, NULL);
//
//	if (r == MSK_RES_OK)
//	{
//		/* Create the optimization task. */
//		r = MSK_maketask(env, NUMCON, NUMVAR, &task);
//
//		if (r == MSK_RES_OK)
//		{
//			/* Directs the log task stream to the 'printstr' function. */
//			r = MSK_linkfunctotaskstream(task, MSK_STREAM_LOG, NULL, printstr);
//
//			/* Append 'NUMCON' empty constraints.
//			The constraints will initially have no bounds. */
//			if (r == MSK_RES_OK)
//				r = MSK_appendcons(task, NUMCON);
//
//			/* Append 'NUMVAR' variables.
//			The variables will initially be fixed at zero (x=0). */
//			if (r == MSK_RES_OK)
//				r = MSK_appendvars(task, NUMVAR);
//
//			/* Optionally add a constant term to the objective. */
//			if (r == MSK_RES_OK)
//				r = MSK_putcfix(task, cfixed);
//
//			for (j = 0; j < NUMVAR && r == MSK_RES_OK; ++j)
//			{
//				/* Set the linear term c_j in the objective.*/
//				if (r == MSK_RES_OK)
//					r = MSK_putcj(task, j, c[j]);
//
//				/* Set the bounds on variable j.
//				blx[j] <= x_j <= bux[j] */
//				if (r == MSK_RES_OK)
//					r = MSK_putvarbound(task,
//					j,           /* Index of variable.*/
//					bkx[j],      /* Bound key.*/
//					blx[j],      /* Numerical value of lower bound.*/
//					bux[j]);     /* Numerical value of upper bound.*/
//
//				/* Input column j of A */
//				if (r == MSK_RES_OK)
//					r = MSK_putacol(task,
//					j,                 /* Variable (column) index.*/
//					aptre[j] - aptrb[j], /* Number of non-zeros in column j.*/
//					asub + aptrb[j],   /* Pointer to row indexes of column j.*/
//					aval + aptrb[j]);  /* Pointer to Values of column j.*/
//			}
//
//			/* Set the bounds on constraints.
//			for i=1, ...,NUMCON : blc[i] <= constraint i <= buc[i] */
//			for (i = 0; i < NUMCON && r == MSK_RES_OK; ++i)
//				r = MSK_putconbound(task,
//				i,           /* Index of constraint.*/
//				bkc[i],      /* Bound key.*/
//				blc[i],      /* Numerical value of lower bound.*/
//				buc[i]);     /* Numerical value of upper bound.*/
//
//			if (r == MSK_RES_OK)
//			{
//				/*
//				* The lower triangular part of the Q
//				* matrix in the objective is specified.
//				*/
//
//				qsubi[0] = 0;   qsubj[0] = 0;  qval[0] = 2.0;
//				qsubi[1] = 1;   qsubj[1] = 1;  qval[1] = 2.0;
//				qsubi[2] = 2;   qsubj[2] = 2;  qval[2] = 2.0;
//
//				/* Input the Q for the objective. */
//
//				r = MSK_putqobj(task, NUMQNZ, qsubi, qsubj, qval);
//			}
//
//			if (r == MSK_RES_OK)
//			{
//				MSKrescodee trmcode;
//
//				/* Run optimizer */
//				r = MSK_optimizetrm(task, &trmcode);
//
//				/* Print a summary containing information
//				about the solution for debugging purposes*/
//				MSK_solutionsummary(task, MSK_STREAM_MSG);
//
//				if (r == MSK_RES_OK)
//				{
//					MSKsolstae solsta;
//					int j;
//
//					MSK_getsolsta(task, MSK_SOL_ITR, &solsta);
//
//					switch (solsta)
//					{
//					case MSK_SOL_STA_OPTIMAL:
//					case MSK_SOL_STA_NEAR_OPTIMAL:
//						MSK_getxx(task,
//							MSK_SOL_ITR,    /* Request the interior solution. */
//							xx);
//
//						printf("Optimal primal solution\n");
//						for (j = 0; j < NUMVAR; ++j)
//							printf("x[%d]: %e\n", j, xx[j]);
//
//						break;
//
//					case MSK_SOL_STA_DUAL_INFEAS_CER:
//					case MSK_SOL_STA_PRIM_INFEAS_CER:
//					case MSK_SOL_STA_NEAR_DUAL_INFEAS_CER:
//					case MSK_SOL_STA_NEAR_PRIM_INFEAS_CER:
//						printf("Primal or dual infeasibility certificate found.\n");
//						break;
//
//					case MSK_SOL_STA_UNKNOWN:
//						printf("The status of the solution could not be determined.\n");
//						break;
//
//					default:
//						printf("Other solution status.");
//						break;
//					}
//				}
//				else
//				{
//					printf("Error while optimizing.\n");
//				}
//			}
//
//			if (r != MSK_RES_OK)
//			{
//				/* In case of an error print error code and description. */
//				char symname[MSK_MAX_STR_LEN];
//				char desc[MSK_MAX_STR_LEN];
//
//				printf("An error occurred while optimizing.\n");
//				MSK_getcodedesc(r,
//					symname,
//					desc);
//				printf("Error %s - '%s'\n", symname, desc);
//			}
//		}
//		MSK_deletetask(&task);
//	}
//	MSK_deleteenv(&env);
//
//	return (r);
//
//}
//
